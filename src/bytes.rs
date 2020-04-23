// Copyright 2020 Dubiousjim <dubiousjim@gmail.com>. All rights reserved. MIT license.
#![allow(dead_code)]

use std::convert::AsRef;
use std::path::Path;

pub fn longest_from<F: Fn(u8) -> bool>(line: &[u8], start: usize, f: F) -> &[u8] {
  let mut after = line.len();
  for stop in start..after {
    let c = line[stop];
    if !f(c) {
      after = stop;
      break;
    }
  }
  &line[start..after]
}

pub fn get_word(value: &[u8], mut start: usize, word: usize) -> Option<(&[u8], usize)> {
  let mut after = start;
  let mut words_started: usize = 0;
  for i in start..value.len() {
    if value[i].is_ascii_whitespace() {
      if after > start {
        if words_started > word {
          break;
        }
        start = after;
      }
    } else {
      if after == start {
        start = i;
        words_started += 1;
      }
      after = i + 1;
    }
  }
  if after > start {
    Some((&value[start..after], after))
  } else {
    None
  }
}

pub fn get_words<'a>(value: &'a [u8], words: &[usize]) -> Option<Vec<&'a [u8]>> {
  let mut next_word: usize = 0; // 0-based
  let mut start: usize = 0;
  let mut res = Vec::with_capacity(words.len());
  for word in words {
    let word = *word - next_word; // also 0-based
                                  // match get_word(&value[start..], 0, word) {
                                  // Some((bytes, after)) => {
                                  // start += after;
    match get_word(&value, start, word) {
      Some((bytes, after)) => {
        start = after;
        next_word += word + 1;
        res.push(bytes);
      }
      None => return None,
    }
  }
  Some(res)
}

pub fn strip_quotes(value: &[u8]) -> &[u8] {
  let len = value.len();
  if len >= 2 && value[0] == b'"' && value[len - 1] == b'"' {
    return &value[1..len - 1];
  }
  value
}

pub fn trim_start(value: &[u8]) -> &[u8] {
  let mut i = 0;
  for j in value.iter() {
    if !j.is_ascii_whitespace() {
      break;
    }
    i += 1;
  }
  &value[i..]
}

pub fn strip_newline(value: &[u8]) -> &[u8] {
  let len = value.len();
  if len >= 1 && (value[len - 1] == 10 || value[len - 1] == 13) {
    return &value[..len - 1];
  }
  value
}

pub fn pop_newline(mut value: Vec<u8>) -> Vec<u8> {
  match value.pop() {
    Some(b) if b != 10 => value.push(b),
    _ => (),
  }
  value
}

pub fn read_all<P: AsRef<Path>>(path: P) -> std::io::Result<Vec<u8>> {
  use std::fs::File;
  use std::io::Read;
  let mut f = File::open(path)?;
  let mut buffer = Vec::new();
  f.read_to_end(&mut buffer)?;
  Ok(buffer)
}

pub fn write_all<P: AsRef<Path>>(path: P, bytes: &[u8]) -> std::io::Result<()> {
  use std::fs::File;
  use std::io::Write;
  let mut f = File::create(path)?; // .write(true).truncate(true).create(true)
  f.write_all(bytes)?;
  Ok(())
}
