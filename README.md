# Spoofed-UDP-Flood

A proof-of-concept demonstrating spoofed UDP flooding in C with randomized payloads and source IP addresses. Supports multi-threading for increased packet generation capacity.

# Features

* Source IP Spoofing: Masks the true origin of the attack.
* Payload Randomization: Helps to evade simple detection mechanisms.
* Multi-threading: Optionally distribute flood traffic generation for greater intensity.

# Compilation & Execution

```bash
gcc udp-spoof.c -o udp-spoof -lpthread
```

The code runs this way:

```bash
./udp-spoof targetip targetport threads time
```
Here is an example:

```bash
./udp-spoof 1.1.1.1 80 5 60
```

# Disclaimer

This project is intended solely for educational and research purposes within controlled environments. UDP flood attacks are illegal and can cause significant disruption to online services.

# Understanding the Code

While the code includes some friendly commentary, studying network protocols and packet structure will enhance your understanding of its operation.

# Notes

* This is a proof-of-concept; a robust UDP flood tool would likely incorporate more sophisticated techniques.
* Always test responsibly, and never target systems you don't have explicit permission to test against.
* Let's Improve It!

I'm open to contributions and suggestions. Feel free to submit issues or pull requests if you have ideas to make this PoC even better.
