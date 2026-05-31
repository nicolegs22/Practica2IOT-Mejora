import socket
import threading
import json
from client_manager import ClientManager
from protocol import Protocol


class IoTServer:
    def __init__(self, host='0.0.0.0', port=5000, puerto=None):
        if puerto is not None:
            port = puerto

        self.host = host
        self.port = port
        self.puerto = port
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.server = self.server_socket
        self.client_manager = ClientManager()
        self.gestor = self.client_manager
        self.current_led_rgb = None
        self.current_led_duration = None
        self.pending_led_rgb = None
        self.pending_led_duration = None
        self.pending_led_count = 0
        self.required_stable_readings = 3

    def start(self):
        self.server_socket.bind((self.host, self.port))
        self.server_socket.listen(5)
        print(f"IoT server started on {self.host}:{self.port}")
        print("-" * 50)

        while True:
            client_socket, address = self.server_socket.accept()
            print(f"[TCP] Connection from {address[0]}:{address[1]}")
            threading.Thread(
                target=self.handle_client,
                args=(client_socket, address),
                daemon=True
            ).start()

    def handle_client(self, client_socket, address):
        buffer = ""
        try:
            while True:
                data = client_socket.recv(1024).decode()
                if not data:
                    break

                buffer += data
                while "\n" in buffer:
                    line, buffer = buffer.split("\n", 1)
                    self.process_message(line, client_socket, address)
        except (ConnectionError, OSError, UnicodeDecodeError):
            pass
        finally:
            self.client_manager.remove(client_socket)
            client_socket.close()

    def process_message(self, data, client_socket, address=None):
        try:
            message = json.loads(data)
        except json.JSONDecodeError:
            return

        message_type = Protocol.get_value(
            message,
            Protocol.MESSAGE_TYPE,
            Protocol.LEGACY_MESSAGE_TYPE,
        )
        message_type = Protocol.normalize_message_type(message_type)

        if message_type == Protocol.REGISTER:
            device_type = Protocol.get_value(
                message,
                Protocol.DEVICE_TYPE,
                Protocol.LEGACY_DEVICE_TYPE,
            )
            normalized_device_type = Protocol.normalize_device_type(device_type)
            supports_modern_protocol = (
                Protocol.MESSAGE_TYPE in message or
                Protocol.DEVICE_TYPE in message
            )
            self.client_manager.register(
                message.get("id"),
                client_socket,
                address,
                device_type,
                supports_modern_protocol,
            )
            if (
                normalized_device_type == Protocol.ACTUATOR and
                self.current_led_rgb is not None
            ):
                self.send_led_command(
                    client_socket,
                    supports_modern_protocol,
                    self.current_led_rgb,
                    self.current_led_duration,
                )

        elif message_type == Protocol.SENSOR_DATA:
            distance = Protocol.get_value(
                message,
                Protocol.DISTANCE,
                Protocol.LEGACY_DISTANCE,
            )
            try:
                distance = float(distance)
            except (TypeError, ValueError):
                return

            color, rgb, duration = self.led_state_for_distance(distance)
            print(f"{color:8} | {distance:5.1f} cm")

            self.update_led_state(rgb, duration)

    @staticmethod
    def led_state_for_distance(distance):
        if distance < 10:
            return "RED", [255, 0, 0], 1000
        if 10 <= distance <= 20:
            return "BLUE", [0, 0, 255], 500
        if 20 <= distance <= 30:
            return "GREEN", [0, 255, 0], 200
        return "OFF", [0, 0, 0], 100

    def send_to_actuators(self, command_or_rgb, duration=None):
        for client in self.client_manager.get_actuators():
            if duration is None:
                command = command_or_rgb
            elif client.supports_modern_protocol:
                command = Protocol.led_command(command_or_rgb, duration)
            else:
                command = Protocol.legacy_led_command(command_or_rgb, duration)

            try:
                client.socket.send((json.dumps(command) + "\n").encode())
            except OSError:
                pass

    def update_led_state(self, rgb, duration):
        if rgb == self.current_led_rgb:
            self.pending_led_rgb = None
            self.pending_led_duration = None
            self.pending_led_count = 0
            return

        if self.current_led_rgb is None:
            self.current_led_rgb = rgb
            self.current_led_duration = duration
            self.send_to_actuators(rgb, duration)
            return

        if rgb == self.pending_led_rgb:
            self.pending_led_count += 1
        else:
            self.pending_led_rgb = rgb
            self.pending_led_duration = duration
            self.pending_led_count = 1

        if self.pending_led_count < self.required_stable_readings:
            return

        self.current_led_rgb = self.pending_led_rgb
        self.current_led_duration = self.pending_led_duration
        self.pending_led_rgb = None
        self.pending_led_duration = None
        self.pending_led_count = 0
        self.send_to_actuators(self.current_led_rgb, self.current_led_duration)

    def send_led_command(self, client_socket, supports_modern_protocol, rgb, duration):
        if supports_modern_protocol:
            command = Protocol.led_command(rgb, duration)
        else:
            command = Protocol.legacy_led_command(rgb, duration)

        try:
            client_socket.send((json.dumps(command) + "\n").encode())
        except OSError:
            pass

    # Backward-compatible method names from the previous Spanish API.
    iniciar = start
    manejar_cliente = handle_client
    procesar_mensaje = process_message
    enviar_a_actuadores = send_to_actuators


ServidorIoT = IoTServer
