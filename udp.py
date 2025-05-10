import os

if __name__ == "__main__":
    # This is a script that can be run directly
    # It will import the UDP class and run the main function
    from .udp import UDP

    def main():
        udp = UDP()
        udp.run()

    main()
