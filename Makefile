PYTHON ?= python3
all:
	$(PYTHON) game/build.py build
context:
	$(PYTHON) game/build.py context
impact:
	$(PYTHON) game/build.py impact $(ID)
economy:
	$(PYTHON) game/build.py economy $(ID)
status:
	$(PYTHON) game/build.py status
check:
	$(PYTHON) game/build.py check
rom:
	$(PYTHON) game/build.py rom
clean:
	rm -rf build
.PHONY: all context impact economy check status rom clean
