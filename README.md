WPPPacketSplitter
=================

This program can analyse multiple .pkt files (in the PKT format), group packets by opcode and create new sniff files so that each file only has packets of a single opcode.

If possible, WPPPacketSplitter will parse WowPacketParser's Opcode.cs files to do a mapping of "build<->opcode id<->opcode name" so that the created files are correctly named with opcodes name.


TODO:
-----

- Remove hardcoded paths and builds

- Sometimes the created files cannot be parsed by WPP (unknown reason)

- Speed up processing and make the program multithreaded

- Make it memory efficient (I tried)
