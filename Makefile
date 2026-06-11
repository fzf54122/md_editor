SHELL := /bin/sh

TAURI_MANIFEST := src-tauri/Cargo.toml
WEBSITE_DIR := website

.PHONY: help install dev fmt fmt-fix rust-check clippy test check build website website-build clean

help:
	@printf '%s\n' 'md-editor targets:'
	@printf '%s\n' '  make install       Install desktop frontend dependencies'
	@printf '%s\n' '  make dev           Run the Tauri desktop app'
	@printf '%s\n' '  make fmt           Check Rust formatting'
	@printf '%s\n' '  make fmt-fix       Format Rust source files'
	@printf '%s\n' '  make rust-check    Run cargo check for the Tauri shell'
	@printf '%s\n' '  make clippy        Run clippy with warnings denied'
	@printf '%s\n' '  make test          Run Rust tests'
	@printf '%s\n' '  make check         Run fmt, rust-check, clippy, and tests'
	@printf '%s\n' '  make build         Compile the local release binary only'
	@printf '%s\n' '  make website       Run the website dev server'
	@printf '%s\n' '  make website-build Build the website'
	@printf '%s\n' '  make clean         Clean Rust build artifacts'

install:
	pnpm install

fmt:
	cargo fmt --manifest-path $(TAURI_MANIFEST) --all --check

fmt-fix:
	cargo fmt --manifest-path $(TAURI_MANIFEST) --all

rust-check:
	cargo check --manifest-path $(TAURI_MANIFEST)

clippy:
	cargo clippy --manifest-path $(TAURI_MANIFEST) --all-targets -- -D warnings

test:
	cargo test --manifest-path $(TAURI_MANIFEST)

check: fmt rust-check clippy test

dev:
	pnpm tauri dev

build:
	cargo build --manifest-path $(TAURI_MANIFEST) --release

website:
	$(MAKE) -C $(WEBSITE_DIR) dev

website-build:
	$(MAKE) -C $(WEBSITE_DIR) build

clean:
	cargo clean --manifest-path $(TAURI_MANIFEST)
