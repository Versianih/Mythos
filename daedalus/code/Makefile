FQBN := esp32:esp32:esp32

PORT ?= /dev/ttyUSB0

SKETCH := .

BAUDRATE := 115200

.PHONY: help ports compile upload monitor flash clean

help:
	@echo "  make ports  : Lista as portas seriais/COM disponíveis"
	@echo "  make compile     : Compila o projeto"
	@echo "  make upload      : Faz o upload para a placa"
	@echo "  make monitor     : Abre o monitor serial"
	@echo "  make flash       : Compila e faz o upload em um só comando"
	@echo "  make clean       : Limpa os arquivos de compilação"

ports:
	arduino-cli board list

compile:
	arduino-cli compile --fqbn $(FQBN) $(BUILD_FLAGS) $(SKETCH)

upload:
	arduino-cli upload -p $(PORT) --fqbn $(FQBN) $(SKETCH)

monitor:
	arduino-cli monitor -p $(PORT) --config baudrate=$(BAUDRATE)

flash: compile upload

clean:
	arduino-cli cache clean
	rm -rf build