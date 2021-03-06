EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 9
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Sheet
S 1275 1325 725  900 
U 606B7288
F0 "max31855" 50
F1 "max31855.sch" 50
F2 "CLK" I B 1825 2225 50 
F3 "~CS~" I B 1375 2225 50 
F4 "MISO" O B 1600 2225 50 
$EndSheet
$Sheet
S 3600 1325 725  900 
U 606BAB07
F0 "sheet606BAB02" 50
F1 "max31855.sch" 50
F2 "CLK" I B 4150 2225 50 
F3 "~CS~" I B 3700 2225 50 
F4 "MISO" O B 3925 2225 50 
$EndSheet
$Sheet
S 4800 1325 725  900 
U 606BABA5
F0 "sheet606BABA0" 50
F1 "max31855.sch" 50
F2 "CLK" I B 5350 2225 50 
F3 "~CS~" I B 4900 2225 50 
F4 "MISO" O B 5125 2225 50 
$EndSheet
$Sheet
S 6050 1325 725  900 
U 606BAC1B
F0 "sheet606BAC16" 50
F1 "max31855.sch" 50
F2 "CLK" I B 6600 2225 50 
F3 "~CS~" I B 6150 2225 50 
F4 "MISO" O B 6375 2225 50 
$EndSheet
$Sheet
S 7250 1325 725  900 
U 606BAC89
F0 "sheet606BAC84" 50
F1 "max31855.sch" 50
F2 "CLK" I B 7800 2225 50 
F3 "~CS~" I B 7350 2225 50 
F4 "MISO" O B 7575 2225 50 
$EndSheet
$Sheet
S 8400 1325 725  900 
U 606BACDB
F0 "sheet606BACD6" 50
F1 "max31855.sch" 50
F2 "CLK" I B 8950 2225 50 
F3 "~CS~" I B 8500 2225 50 
F4 "MISO" O B 8725 2225 50 
$EndSheet
$Sheet
S 9500 1325 725  900 
U 606BAD2D
F0 "sheet606BAD28" 50
F1 "max31855.sch" 50
F2 "CLK" I B 10050 2225 50 
F3 "~CS~" I B 9600 2225 50 
F4 "MISO" O B 9825 2225 50 
$EndSheet
$Comp
L power:GND #PWR02
U 1 1 606BD9E1
P 7150 3925
F 0 "#PWR02" H 7150 3675 50  0001 C CNN
F 1 "GND" H 7155 3752 50  0000 C CNN
F 2 "" H 7150 3925 50  0001 C CNN
F 3 "" H 7150 3925 50  0001 C CNN
	1    7150 3925
	1    0    0    -1  
$EndComp
Wire Wire Line
	7150 3925 6900 3925
Wire Wire Line
	10050 3150 10050 2225
Wire Wire Line
	8950 2225 8950 3150
Connection ~ 8950 3150
Wire Wire Line
	8950 3150 10050 3150
Wire Wire Line
	7800 2225 7800 3150
Connection ~ 7800 3150
Wire Wire Line
	7800 3150 8950 3150
Wire Wire Line
	6600 2225 6600 3150
Connection ~ 6600 3150
Wire Wire Line
	6600 3150 7800 3150
Wire Wire Line
	5350 2225 5350 3150
Wire Wire Line
	4150 2225 4150 3150
Wire Wire Line
	4150 3150 5350 3150
Connection ~ 5350 3150
Wire Wire Line
	3000 2225 3000 3150
Wire Wire Line
	3000 3150 4150 3150
Connection ~ 4150 3150
Wire Wire Line
	1825 2225 1825 3150
Wire Wire Line
	1825 3150 3000 3150
Connection ~ 3000 3150
Wire Wire Line
	1600 2225 1600 3050
Wire Wire Line
	1600 3050 2775 3050
Wire Wire Line
	2775 2225 2775 3050
Connection ~ 2775 3050
Wire Wire Line
	2775 3050 3925 3050
Wire Wire Line
	3925 2225 3925 3050
Connection ~ 3925 3050
Wire Wire Line
	3925 3050 5125 3050
Wire Wire Line
	5125 2225 5125 3050
Connection ~ 5125 3050
Wire Wire Line
	6375 2225 6375 3050
Wire Wire Line
	9825 3050 9825 2225
Connection ~ 6375 3050
Wire Wire Line
	8725 2225 8725 3050
Connection ~ 8725 3050
Wire Wire Line
	8725 3050 9825 3050
Wire Wire Line
	7575 2225 7575 3050
Wire Wire Line
	6375 3050 7575 3050
Connection ~ 7575 3050
Wire Wire Line
	7575 3050 8725 3050
Wire Wire Line
	9600 2225 9600 2975
Wire Wire Line
	9600 2975 6175 2975
Wire Wire Line
	6175 2975 6175 3925
Wire Wire Line
	8500 2225 8500 2900
Wire Wire Line
	8500 2900 6075 2900
Wire Wire Line
	6075 2900 6075 3925
Wire Wire Line
	7350 2225 7350 2825
Wire Wire Line
	7350 2825 5975 2825
Wire Wire Line
	5975 2825 5975 3925
Wire Wire Line
	6150 2225 6150 2750
Wire Wire Line
	6150 2750 5875 2750
Wire Wire Line
	5875 2750 5875 3925
Wire Wire Line
	4900 2225 4900 2750
Wire Wire Line
	4900 2750 5775 2750
Wire Wire Line
	5775 2750 5775 3925
Wire Wire Line
	3700 2225 3700 2825
Wire Wire Line
	3700 2825 5675 2825
Wire Wire Line
	5675 2825 5675 3925
Wire Wire Line
	2550 2225 2550 2925
Wire Wire Line
	2550 2925 5575 2925
Wire Wire Line
	5575 2925 5575 3925
Wire Wire Line
	1375 2225 1375 3250
Wire Wire Line
	1375 3250 5475 3250
Wire Wire Line
	5475 3250 5475 3925
$Comp
L power:PWR_FLAG #FLG02
U 1 1 606C4E21
P 6900 3925
F 0 "#FLG02" H 6900 4000 50  0001 C CNN
F 1 "PWR_FLAG" H 6900 4098 50  0000 C CNN
F 2 "" H 6900 3925 50  0001 C CNN
F 3 "~" H 6900 3925 50  0001 C CNN
	1    6900 3925
	1    0    0    -1  
$EndComp
Connection ~ 6900 3925
Wire Wire Line
	5125 3050 6375 3050
$Sheet
S 2450 1325 725  900 
U 606BA995
F0 "sheet606BA990" 50
F1 "max31855.sch" 50
F2 "CLK" I B 3000 2225 50 
F3 "~CS~" I B 2550 2225 50 
F4 "MISO" O B 2775 2225 50 
$EndSheet
Wire Wire Line
	6875 3525 7150 3525
Connection ~ 6875 3525
$Comp
L power:PWR_FLAG #FLG01
U 1 1 606C4A69
P 6875 3525
F 0 "#FLG01" H 6875 3600 50  0001 C CNN
F 1 "PWR_FLAG" H 6875 3698 50  0000 C CNN
F 2 "" H 6875 3525 50  0001 C CNN
F 3 "~" H 6875 3525 50  0001 C CNN
	1    6875 3525
	1    0    0    -1  
$EndComp
Wire Wire Line
	6475 3525 6875 3525
$Comp
L power:+3.3V #PWR01
U 1 1 606BDE49
P 7150 3525
F 0 "#PWR01" H 7150 3375 50  0001 C CNN
F 1 "+3.3V" H 7165 3698 50  0000 C CNN
F 2 "" H 7150 3525 50  0001 C CNN
F 3 "" H 7150 3525 50  0001 C CNN
	1    7150 3525
	1    0    0    -1  
$EndComp
Wire Wire Line
	6275 3150 6600 3150
Wire Wire Line
	5350 3150 6275 3150
Connection ~ 6275 3150
$Comp
L Connector_Generic:Conn_01x08 J1
U 1 1 606FD2CB
P 5875 4125
F 0 "J1" V 5747 4505 50  0000 L CNN
F 1 "Conn_01x08" V 5838 4505 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x08_P2.54mm_Vertical" H 5875 4125 50  0001 C CNN
F 3 "~" H 5875 4125 50  0001 C CNN
	1    5875 4125
	0    1    1    0   
$EndComp
Wire Wire Line
	6275 3925 6275 3150
Wire Wire Line
	6375 3050 6375 3925
Wire Wire Line
	6475 3925 6475 3525
Wire Wire Line
	6900 3925 6575 3925
$Comp
L Connector_Generic:Conn_01x04 J2
U 1 1 606FC737
P 6475 4125
F 0 "J2" V 6347 4305 50  0000 L CNN
F 1 "Conn_01x04" V 6438 4305 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x04_P2.54mm_Vertical" H 6475 4125 50  0001 C CNN
F 3 "~" H 6475 4125 50  0001 C CNN
	1    6475 4125
	0    1    1    0   
$EndComp
$Comp
L Thermo:MountingHoles H1
U 1 1 60746452
P 1350 5570
F 0 "H1" H 1445 5541 50  0000 L CNN
F 1 "MountingHoles" H 1445 5450 50  0000 L CNN
F 2 "Thermo:MountingHole_pattern" H 1400 5920 50  0001 C CNN
F 3 "" H 1400 5920 50  0001 C CNN
	1    1350 5570
	1    0    0    -1  
$EndComp
$EndSCHEMATC
