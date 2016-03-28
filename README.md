# EA-MITM

MITM for EA's DirtySock/ProtoSSL/ProtoHTTP powered products, fairly simple, used for dumping data between secure connections.

Click (here)[http://blog.igonline.eu/ea-man-in-the-middle-ea-mitm-32/] for more information

## Installation/Usage

NOTE: Make sure you have Visual Studio installed on your system.

1. Download, or clone repository using `git clone https://github.com/Imposter/EA-MITM.git`
2. Use the `EA-MITM.sln` to open the solution in Visual Studio to build it.
2. Once you've opened the solution, select either x86/x64 for the target platform. (x64 is untested and it's likely it will not work!) 
`NOTE: Debug builds will have a console (and will break once the DLL is loaded) while release builds will not.`
3. Compile the project and copy the outputted .dll file to the game's directory as dinput8.dll (could be different depending on what is exported). Do note, this will not work out of the box for all EA games, as some will require slight modification of the code. For example, the Need for Speed: Rivals packer loads the game differently than Need for Speed: Most Wanted or Battlefield 3 and the dinput8.dll we've created will not be loaded. 
4. Create a EA-MITM.ini file in the root folder of the game with the following contents (for Need for Speed: Most Wanted, the rest of the offsets can be obtained in Offsets.txt):

```
[Hook]
InitDelay=5000

[ProtoSSL]
Connect=0x00B91930
Send=0x00B92FC0
Recv=0x00B93070
```

5. Launch and play the game. The captured packets will be written to protossl_dump_*.acp and can be viewed using Wireshark.

## Disclaimer

This patch was created with good intentions and is by no means a crack, bypass or other unlawful program. This program is intended for educational purposes only. By using this program, you agree that I am not responsible for your personal choices or actions before, during or after its use. 

## Copyright

Is something infringing your copyright? Get in touch [here](http://blog.igonline.eu/contact/) and I'll get rid of it as soon as possible!

## License

Copyright (C) 2016 Eyaz Rehman. All Rights Reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or any 
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.