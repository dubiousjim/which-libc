fn main() {
  // println!("cargo:rustc-env=HOST={}", std::env::var("HOST").expect("$HOST"));
  // println!("cargo:rustc-env=TARGET={}", std::env::var("TARGET").expect("$TARGET"));
  println!("cargo:rerun-if-changed=build.rs");
  println!("cargo:rerun-if-changed=src/macro.h");
  println!("cargo:rerun-if-changed=src/detect.h");
  println!("cargo:rerun-if-changed=src/detect.c");
  println!("cargo:rerun-if-env-changed=TARGET");
  cc::Build::new().file("src/detect.c").compile("detect");
}
