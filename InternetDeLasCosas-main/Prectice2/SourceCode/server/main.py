import argparse

from server import IoTServer


def parse_args():
    parser = argparse.ArgumentParser(description="Run the IoT TCP server.")
    parser.add_argument("--host", default="0.0.0.0", help="IP address to bind the TCP server.")
    parser.add_argument("--port", type=int, default=5000, help="TCP port to listen on.")
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_args()
    server = IoTServer(host=args.host, port=args.port)
    try:
        server.start()
    except KeyboardInterrupt:
        print("Shutting down server...")
