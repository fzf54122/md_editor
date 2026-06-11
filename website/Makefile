SHELL := /bin/sh

.PHONY: help install dev build preview clean

help:
	@printf '%s\n' 'md-editor website targets:'
	@printf '%s\n' '  make install  Install dependencies'
	@printf '%s\n' '  make dev      Run local website server'
	@printf '%s\n' '  make build    Build static website'
	@printf '%s\n' '  make preview  Preview built website'
	@printf '%s\n' '  make clean    Remove dist output'

install:
	pnpm install

dev:
	pnpm dev

build:
	pnpm build

preview:
	pnpm preview

clean:
	rm -rf dist
