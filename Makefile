push: build
	rsync -vaP --delete book/ caos.myltsev.ru:lectures/

.PHONY: build serve
build:
	mdbook build

serve:
	mdbook serve
