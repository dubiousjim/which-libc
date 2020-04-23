// Copyright 2020 Dubiousjim <dubiousjim@gmail.com>. All rights reserved. MIT license.
//
// References
// https://github.com/schultyy/os_type
// https://benohead.com/blog/2015/01/28/linux-check-glibc-version/
// https://github.com/rust-lang/rust/issues/71564
// https://docs.rs/minreq/2.1.0/minreq/

// extern crate version_check as rustc;
use std::process::Command;

mod bytes;
// TODO
use bytes::{longest_from, read_all, write_all};

#[derive(Debug, Clone, Copy, PartialEq)]
pub enum Environment {
  Mac(u32),
  Gnu(u32),
  Musl((u32, u32)),
  Android(u32),
  FreeBSD(u32),
  NetBSD((u32, u32)),
  Windows,
  Unknown,
}

impl std::fmt::Display for Environment {
  fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
    match self {
      Environment::Mac(y) => write!(f, "Mac 10.{}", y),
      Environment::Gnu(y) => write!(f, "Gnu libc 2.{}", y),
      Environment::Musl((y, z)) => write!(f, "Musl libc 1.{}.{}", y, z),
      Environment::Android(x) => write!(f, "Android {}", x),
      Environment::FreeBSD(x) => write!(f, "FreeBSD {}", x),
      Environment::NetBSD((x, y)) => write!(f, "NetBSD {}.{}", x, y),
      Environment::Windows => write!(f, "Windows"),
      Environment::Unknown => write!(f, "Unknown libc"),
    }
  }
}

// from https://github.com/SergioBenitez/version_check
fn get_rustc_version() -> Option<String> {
  std::env::var("RUSTC")
    .ok()
    .and_then(|rustc| Command::new(rustc).arg("--version").output().ok())
    .or_else(|| Command::new("rustc").arg("--version").output().ok())
    .and_then(|output| String::from_utf8(output.stdout).ok())
    .as_ref()
    .and_then(|s| {
      let last_line = s.lines().last().unwrap_or(s);
      let mut components = last_line.trim().split(" ");
      let version = components.nth(1);
      version.map(|s| s.to_string())
    })
}

fn fetch<T: Into<minreq::URL>>(url: T, timeout: Option<u32>, cache: Option<&str>) -> Option<Vec<u8>> {
  let mut file_path = None;
  if let Some(cache_path) = cache {
    let out_dir = std::env::var("OUT_DIR").expect("unset $OUT_DIR");
    let path = format!("{}/{}", out_dir, cache_path);
    if let Ok(vec) = read_all(&path) {
      return Some(vec);
    }
    file_path = Some(path);
  }
  if let Some(timeout) = timeout {
    std::env::set_var("MINREQ_TIMEOUT", timeout.to_string()); // or use .with_timeout(10) before send()
  }
  minreq::get(url).send().ok().and_then(|response| {
    if response.status_code == 200 {
      assert_eq!(response.reason_phrase, "OK");
      let bytes = response.into_bytes();
      if let Some(file_path) = file_path {
        if write_all(&file_path, &bytes).is_err() {
          let _ = std::fs::remove_file(&file_path).is_ok();
        }
      }
      Some(bytes)
    } else {
      None
    }
  })
}

fn get_musl_version() -> Option<String> {
  let rustc_version = get_rustc_version().expect("unknown rustc version");
  let cache_path = format!("musl-for-rustc-{}", rustc_version);
  let script_path = "src/ci/docker/scripts/musl.sh";
  let url = format!(
    "https://github.com/rust-lang/rust/blob/{}/{}?raw=true",
    rustc_version, script_path
  );
  fetch(url, Some(10), Some(&cache_path)).and_then(|bytes| {
    for line in bytes.split(|c| *c == 10 || *c == 13) {
      if line.starts_with(b"MUSL=") {
        assert_eq!(&line[5..10], b"musl-");
        let version = longest_from(line, 10, |c| c == b'.' || (c >= b'0' && c <= b'9'));
        return String::from_utf8(version.to_vec()).ok();
      }
    }
    None
  })
}

fn parse_version(ver: &str) -> (u32, u32, u32) {
  let mut pieces = ver.split('.');
  use std::str::FromStr;
  // let major = u32::from_str_radix(pieces.next().unwrap(), 10).unwrap();
  let major = u32::from_str(pieces.next().unwrap()).unwrap();
  let minor = u32::from_str(pieces.next().unwrap()).unwrap();
  let patch = u32::from_str(pieces.next().unwrap()).unwrap();
  (major, minor, patch)
}

extern "C" {
  static target_os: *const std::os::raw::c_char;
  static target_version: u64;
}

pub fn detect() -> Environment {
  let os = unsafe { std::ffi::CStr::from_ptr(target_os) };
  let os: Vec<u8> = os.to_bytes().to_vec();
  let version = unsafe { target_version };

  assert_eq!(os == b"Windows", cfg!(windows)); // target_env = "msvc"? or "mingw"??
  assert_eq!(os == b"Android", cfg!(target_os = "android"));
  assert_eq!(os == b"FreeBSD", cfg!(target_os = "freebsd"));
  assert_eq!(os == b"NetBSD", cfg!(target_os = "netbsd"));
  assert_eq!(os == b"OpenBSD", cfg!(target_os = "openbsd"));
  assert_eq!(os == b"DragonFly", cfg!(target_os = "dragonfly"));
  assert_eq!(os == b"OSX", cfg!(target_os = "macos"));
  assert_eq!(os == b"Gnu/Linux", cfg!(all(target_os = "linux", target_env = "gnu")));
  assert_eq!(os == b"Musl/Linux", cfg!(all(target_os = "linux", target_env = "musl")));
  assert_eq!(
    os == b"Linux",
    cfg!(all(
      target_os = "linux",
      not(any(target_env = "gnu", target_env = "musl"))
    ))
  );
  /* target_os may also be "ios" */

  if os == b"Windows" {
    Environment::Windows
  } else if os == b"MSYS" {
    Environment::Unknown
  } else if os == b"Cygwin" {
    Environment::Unknown // version available
  } else if os == b"FreeBSD" {
    Environment::FreeBSD((version / 100) as u32)
  } else if os == b"NetBSD" {
    Environment::NetBSD(((version / 100) as u32, (version % 100) as u32))
  } else if os == b"OpenBSD" {
    Environment::Unknown // version available
  } else if os == b"DragonFly" {
    Environment::Unknown
  } else if os == b"OSX" {
    assert_eq!(version/100, 10);
    Environment::Mac((version - 1000) as u32) // 10._
  } else if os == b"Android" {
    Environment::Android(version as u32)
  } else if os == b"Gnu/Linux" {
    assert_eq!(version/100, 2);
    Environment::Gnu((version - 200) as u32) // 2._
  } else if os == b"Musl/Linux" {
    let version = get_musl_version().expect("unknown Musl version");
    let (major, minor, patch) = parse_version(&version);
    assert_eq!(major, 1);
    Environment::Musl((minor, patch)) // 1._._
  } else {
    /* os == "Linux" or "" */
    Environment::Unknown
  }
}

#[cfg(test)]
mod tests {
  use super::*;

  #[test]
  fn test_detect() {
    println!("Detected {}", detect());
  }
}
