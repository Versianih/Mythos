import asyncio
import json
import sys
from pathlib import Path

import pygame
from bleak import BleakClient, BleakScanner

DEVICE_NAME = "HORUS"
CHAR_DRIVE_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"

CONFIG_PATH = Path(__file__).parent / "controller_config.json"

DEFAULT_CONFIG = {
    "drive_axis_x": 0,
    "drive_axis_y": 1,
    "invert_x": False,
    "invert_y": True,
    "deadzone": 0.15,
    "max_speed": 100,
    "send_rate_hz": 20,
    "stop_button": None,
}


def load_config():
    if CONFIG_PATH.exists():
        return {**DEFAULT_CONFIG, **json.loads(CONFIG_PATH.read_text())}
    save_config(DEFAULT_CONFIG)
    return dict(DEFAULT_CONFIG)


def save_config(config):
    CONFIG_PATH.write_text(json.dumps(config, indent=4))


def apply_deadzone(value, deadzone):
    if abs(value) < deadzone:
        return 0.0
    sign = 1 if value > 0 else -1
    return sign * (abs(value) - deadzone) / (1 - deadzone)


def tank_mix(x, y, max_speed):
    left = y + x
    right = y - x
    peak = max(abs(left), abs(right), 1.0)
    if peak > 1.0:
        left /= peak
        right /= peak
    return int(left * max_speed), int(right * max_speed)


def find_joystick():
    pygame.joystick.init()
    if pygame.joystick.get_count() == 0:
        return None
    joystick = pygame.joystick.Joystick(0)
    joystick.init()
    return joystick


def detect_axis(joystick, exclude_index=None, threshold=0.6):
    while True:
        pygame.event.pump()
        for i in range(joystick.get_numaxes()):
            if i == exclude_index:
                continue
            value = joystick.get_axis(i)
            if abs(value) > threshold:
                pygame.time.wait(300)
                return i, value < 0
        pygame.time.wait(20)


def detect_button(joystick, timeout_ms=8000):
    waited = 0
    while waited < timeout_ms:
        pygame.event.pump()
        for i in range(joystick.get_numbuttons()):
            if joystick.get_button(i):
                pygame.time.wait(200)
                return i
        pygame.time.wait(20)
        waited += 20
    return None


def configure(joystick):
    print("=== Configuracao de controle ===")
    print("Mova o eixo horizontal (esquerda/direita) do analogico de direcao...")
    axis_x, invert_x = detect_axis(joystick)

    print("Mova o eixo vertical (cima/baixo) do analogico de direcao...")
    axis_y, invert_y = detect_axis(joystick, exclude_index=axis_x)

    print("Aperte o botao de parada de emergencia (ou espere 8s para pular)...")
    stop_button = detect_button(joystick)

    config = load_config()
    config.update({
        "drive_axis_x": axis_x,
        "drive_axis_y": axis_y,
        "invert_x": invert_x,
        "invert_y": invert_y,
        "stop_button": stop_button,
    })
    save_config(config)
    print(f"Configuracao salva em {CONFIG_PATH}")
    return config


async def find_device():
    print(f"Procurando '{DEVICE_NAME}'...")
    return await BleakScanner.find_device_by_name(DEVICE_NAME, timeout=10.0)


def read_drive(joystick, config):
    x = joystick.get_axis(config["drive_axis_x"])
    y = joystick.get_axis(config["drive_axis_y"])

    if config["invert_x"]:
        x = -x
    if config["invert_y"]:
        y = -y

    stop_button = config.get("stop_button")
    if stop_button is not None and joystick.get_button(stop_button):
        return 0, 0

    x = apply_deadzone(x, config["deadzone"])
    y = apply_deadzone(y, config["deadzone"])

    return tank_mix(x, y, config["max_speed"])


async def run(joystick, config):
    period = 1.0 / config["send_rate_hz"]

    while True:
        device = await find_device()
        if device is None:
            print("Robo nao encontrado, tentando novamente...")
            await asyncio.sleep(2)
            continue

        try:
            async with BleakClient(device) as client:
                print(f"Conectado ao {DEVICE_NAME}")
                while client.is_connected:
                    pygame.event.pump()

                    left, right = read_drive(joystick, config)
                    payload = bytes([left & 0xFF, right & 0xFF])
                    await client.write_gatt_char(CHAR_DRIVE_UUID, payload, response=False)

                    await asyncio.sleep(period)
        except Exception as exc:
            print(f"Conexao perdida: {exc}")
            await asyncio.sleep(1)


def main():
    pygame.init()
    joystick = find_joystick()
    if joystick is None:
        print("Nenhum controle detectado. Conecte o controle (USB ou Bluetooth) e tente de novo.")
        sys.exit(1)

    print(f"Controle detectado: {joystick.get_name()}")

    config = load_config()
    if "--configure" in sys.argv:
        config = configure(joystick)

    try:
        asyncio.run(run(joystick, config))
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    main()