%% RF Study for Iridium Next Payload
% TOLOSAT
% 12/2020
% Link budget
% Code for the calculation of a link budget TOLOSAT (/ground) - Iridium
clearvars
%% INPUTS
uplink = false;
%Average Power during a transmit slot (max)
P_PL = 1.5; %[W]
%Average power iridium 
P_SAT = 1;
%Gain max of the payload/ground/our antenna
%MPA-D254-1621 realized gain
GPL_max = 2.5; %[dBi]
%No clue on how to estimate system noise temperature. We assume this number
%from Iridium budget sheet
GoverT_PL = -31;    %[dB]
%Gain max of Iridium satellite
%? Phased antenna array
GSAT_max = 20.9; %[dB]
%No clue on how to estimate system noise temperature. We assume this number
%from Iridium budget sheet
GoverT_SAT = -11.2;    %[dB]
%Data rate 
%Earth radius
RE = 6378e3 ;  %[m]
%Altitude of Iridium satellite orbit
H = 780e3;  %[m]
%Boltzmann constant
kB = -228.8; %[dBJ/K]
%Bandwidth 
B = 10e6;   %[Hz]
%Symbol rate
SR = 25e3;  %[bps]
%Modulation size
M = 4;
%Elevation angle (case ground to satellite)
E = 23.53*pi/180;     %[rad]
%Difference in true anomalies between TOLOSAT and Iridum next satellite
%(case satellite to satellite)
Dth = 10*pi/180;   %[rad]
%Link frequency
f0 = 1620*1e6;   %[Hz]

%Speed of light
c = 2.998e8;   %[m/s]


%Maximum cable losses admitted in the antenna on ground/Tolosat payload/our (Modem documentation)
Lc_PL = 2;    %[dB]

%Losses in Iridium satellite (no clue how to model this)
Lc_SAT = 0;    %[dB]

%Axial ratio of ground/Tolosat payload/ours
%MPA-D254-1621 typical axial ratio
AR_PL = 4;  %[dB]

%Axial ratio of the Iridium satellite antennas
%Iridium next satellite axial ratio (Iridium constellation document)
AR_SAT = 3.5;  %[dB]

%Atmospheric losses
%Negligible @f0
Latm = 0.33;   %[dB]

%Diverse losses, margin
%Term on modem documentation
Lm_UL = 0;  %[dB]
Lm_DL =0;  %[dB]

%% Uplink or downlink
if(uplink)
    Pin = P_PL;
    Gt_max = GPL_max;
    Gr_max = 0;
    ARt = db2mag(AR_PL);
    ARr = db2mag(AR_SAT);
    Ltx = Lc_PL;
    Lrx = Lc_SAT;
    Lm = Lm_UL;
    GoverT = GoverT_SAT;
else 
    Pin = P_SAT;
    Gt_max = GSAT_max;
    Gr_max = GPL_max;
    ARt = db2mag(AR_SAT);
    ARr = db2mag(AR_PL);
    Ltx = Lc_SAT;
    Lrx = Lc_PL;
    Lm = Lm_DL;
    GoverT = GoverT_PL;
end

%% Preliminary calculations
%Input power conversion
P_PL = db(P_PL,'power');
%Wavelength
lambda = c / f0;
%Calculation of distance transmitter receiver
alpha = asin(RE / (RE+H) * cos(E));
R = sqrt(RE^2 + (RE+H)^2 - 2*RE*(RE+H)*cos(pi/2 - alpha - E));
%For the caase in orbit
%R = sqrt((RE+H1)^2 + (RE+H2)^2 - 2*(RE+H1)*(RE+H2)*cos(Dth));
%Atmospheric loss
Lfs = (4*pi*R)^2 / lambda^2;
Lfs = db(Lfs,'power');

%Polarisation loss
%Bet is the angle between transmitter and receiver ellipses
beta = 90*pi/180;
Lpol = 1/2 + 2*ARt*ARr / ( (1+ ARt^2)*(1+ARr^2)) + ...
    + ((1-ARt^2)*(1-ARr^2) / (2* (1+ARr^2)*(1+ARr^2)))*cos(2*beta);
Lpol = -db(Lpol,'power');



%% Link budget
L =  Lfs + Lpol + Ltx + Lrx + Latm + Lm; % Total loss
Prec = Pin + Gt_max + Gr_max - L; %Link budget equation

db2pow(Prec) %W
pow2db(db2pow(Prec)*1e3) %dBm

CN0 = Pin+ Gt_max + GoverT - L - kB 

EsN0 = CN0 - db(SR,'power')
EbN0 = EsN0 - db(log2(M),'power')

