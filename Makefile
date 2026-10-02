.PHONY: build test image ui clean
build:
	pio run -d firmware
test:
	sh tools/test.sh
image: build
	python3 tools/image.py
ui:
	sh tools/render-ui.sh
	python3 tools/artwork.py
clean:
	pio run -d firmware -t clean
