push: build
	rsync -vaP --delete book/ ejudge:lectures/

.PHONY: build serve
build:
	mdbook build

serve:
	mdbook serve
