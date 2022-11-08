push: build
	rsync -vaP --delete book/ ejudge:lectures/

.PHONY: build
build:
	mdbook build
