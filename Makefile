push: build
	rsync -vaP --delete book/ caos2023.myltsev.ru:lectures/

.PHONY: build serve
build:
	mdbook build

serve:
	mdbook serve
