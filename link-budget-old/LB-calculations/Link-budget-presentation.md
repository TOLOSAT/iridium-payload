This is an attempt to calculate a link budget for the Iridium payload inside of the Tolosat satellite.

# Requirements
Open a terminal 
```bash
pip install numpy
pip install control
pip install astropy
pip install TLE-tools
```

# Hypothesis

## Iridium numerical values 
Parameter | Value | Meaning | Source|
 --- | --- | --- | ---
P_SAT | 1,5 W | Average Power during a transmit slot|NEED TO BE VERIFIED
 Axial ratio | 3.5 |see [DT2] | 
L_SAT

## Tolosat numerical values

Loss | Value (dB) | Meaning | Source|
 --- | --- | --- | ---
 P_PL | 1.5 | Average Power during a transmit slot|[DR1 p25] 
 Axial ratio | 4 | See [DT2] | 
 L_PL | 13 dB | Difference between the received power level (Lm) and the minimum required power level (Dl) at the receiver.

## Losses
Loss | Value (dB) | Meaning | Note|Source
 --- | --- | --- | ---| ---
Lc_SAT | 0 | Loss in the cable between the modem and the antenna (in an Iridium satellite) | neglected
Lc_PL | 2 |  Loss in the cable between the modem and the antenna (in an Iridium satellite) | Maximum value allowed, with more the communication is not guaranteed |[DR1 p25]|
Latm | 0 | Atmospheric losses| Negligibe, there is almost no atmosphere between 400 and 700 km| [DR3]

## Uplink numerical values


## Downlink numerical values





NB: In our case, uplink and downlink meanings are reversed. Indeed, an uplink is a communication between Tolosat and a satellite from The Iridium-Next constellation, ie a Telemetry (TM), while a downlink is an Iridium-Tolosat communication, ie a Telecommand (TC). 


# Reference documents

[DR 1](https://drive.google.com/drive/folders/1e7a18I7tHASb86wTrSjnybxu4zR54TIt) : Iridium 9603N Developper's guide  
[DR 2](https://drive.google.com/drive/folders/1NZUNK1em5qUZ_SwfdnAVFhRPbDNzNcse) : Antenna datasheet  
[DR 3](https://drive.google.com/drive/folders/1C2tu57cuCGXt97GMfiLHbKCA47ofsTC_) : For the downlink: attachment schedule S DOC 32  
[DR 4](https://drive.google.com/drive/folders/1C2tu57cuCGXt97GMfiLHbKCA47ofsTC_) : For the uplink: attachment schedule S DOC 19  
[DR 5](https://docs.google.com/document/d/1JieGs47VJ9E5zeDgZMB69dEmIF8dfO8L/edit) : Very old document but still has some interesting info  
[DR 6](https://drive.google.com/drive/folders/1LY3PSfYB8v93tYUfnRmJDbB21J38bHoW) :   

DR3 and 4 comes from [this website](https://licensing.fcc.gov/cgi-bin/ws.exe/prod/ib/forms/attachment_menu.hts?id_app_num=102703&acct=265137&id_form_num=15&filing_key=-260629)

# Theory documents
[DT 1](https://en.wikipedia.org/wiki/Eb/N0) : Wikipedia page used for the theory.
[DT 2](https://www.antenna-theory.com/definitions/axial.php), Axial ratio explanation

# Coverage part

* [This figure]() explains why we consider this value for the Iridium satelite opening angle and why we get a limitation on the elevation angle of any iridium satellite from Tolosat. 

