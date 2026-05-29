from datetime import datetime
from dataclasses import dataclass, field

from protocol import Protocol


@dataclass
class Client:
    client_id: str
    socket: object
    address: object
    device_type: str
    supports_modern_protocol: bool = False
    connected_at: datetime = field(default_factory=datetime.now)

    @property
    def id(self):
        return self.client_id

    @property
    def direccion(self):
        return self.address

    @property
    def tipo(self):
        return self.device_type

    @property
    def conectado(self):
        return self.connected_at


class ClientManager:
    def __init__(self):
        self.clients = {}
        self.clientes = self.clients

    def register(self, client_id, socket, address, device_type, supports_modern_protocol=False):
        device_type = Protocol.normalize_device_type(device_type)
        self.clients[client_id] = Client(
            client_id,
            socket,
            address,
            device_type,
            supports_modern_protocol,
        )
        print(f"[+] Registered {client_id} ({device_type})")

    def remove(self, socket):
        for client_id, client in list(self.clients.items()):
            if client.socket == socket:
                del self.clients[client_id]
                print(f"[-] Disconnected {client_id}")

    def get_actuators(self):
        return [c for c in self.clients.values() if c.device_type == Protocol.ACTUATOR]

    # Backward-compatible method names from the previous Spanish API.
    registrar = register
    eliminar = remove
    obtener_actuadores = get_actuators


Cliente = Client
GestorClientes = ClientManager
