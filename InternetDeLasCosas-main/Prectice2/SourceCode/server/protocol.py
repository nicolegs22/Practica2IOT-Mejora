from datetime import datetime

class Protocol:
    MESSAGE_TYPE = "message_type"
    DEVICE_TYPE = "device_type"
    COMMAND_NAME = "command"
    DISTANCE = "distance"
    DURATION = "duration"
    RESULT = "result"
    RGB = "rgb"
    TIMESTAMP = "timestamp"

    LEGACY_MESSAGE_TYPE = "tipo"
    LEGACY_DEVICE_TYPE = "dispositivo_tipo"
    LEGACY_COMMAND_NAME = "comando"
    LEGACY_DISTANCE = "distancia"
    LEGACY_DURATION = "duracion"
    LEGACY_RESULT = "resultado"

    REGISTER = "register"
    SENSOR_DATA = "sensor_data"
    COMMAND = "command"
    COMMAND_RESPONSE = "command_response"
    SENSOR = "sensor"
    ACTUATOR = "actuator"

    LEGACY_REGISTER = "registro"
    LEGACY_COMMAND = "comando"
    LEGACY_COMMAND_RESPONSE = "comando_respuesta"
    LEGACY_ACTUATOR = "actuador"

    @staticmethod
    def get_value(payload, primary_key, legacy_key=None, default=None):
        for key in (primary_key, legacy_key):
            if key and key in payload and payload[key] is not None:
                return payload[key]
        return default

    @classmethod
    def normalize_message_type(cls, value):
        aliases = {
            cls.LEGACY_REGISTER: cls.REGISTER,
            cls.LEGACY_COMMAND: cls.COMMAND,
            cls.LEGACY_COMMAND_RESPONSE: cls.COMMAND_RESPONSE,
        }
        return aliases.get(value, value)

    @classmethod
    def normalize_device_type(cls, value):
        aliases = {
            cls.LEGACY_ACTUATOR: cls.ACTUATOR,
        }
        return aliases.get(value, value)

    @classmethod
    def led_command(cls, rgb, duration):
        """Create a command to control RGB LEDs."""
        return {
            cls.MESSAGE_TYPE: cls.COMMAND,
            cls.COMMAND_NAME: "leds",
            cls.RGB: rgb,
            cls.DURATION: duration,
            cls.TIMESTAMP: datetime.now().isoformat(),
        }

    @classmethod
    def legacy_led_command(cls, rgb, duration):
        """Create a command compatible with the previous Spanish protocol."""
        return {
            cls.LEGACY_MESSAGE_TYPE: cls.LEGACY_COMMAND,
            cls.LEGACY_COMMAND_NAME: "leds",
            cls.RGB: rgb,
            cls.LEGACY_DURATION: duration,
            cls.TIMESTAMP: datetime.now().isoformat(),
        }


# Backward-compatible aliases for the previous Spanish API.
Protocolo = Protocol
Protocol.comando_led = Protocol.legacy_led_command
