EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 9 9
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L power:+3.3V #PWR?
U 1 1 606B8091
P 5350 2850
AR Path="/606B8091" Ref="#PWR?"  Part="1" 
AR Path="/606B7288/606B8091" Ref="#PWR04"  Part="1" 
AR Path="/606BA995/606B8091" Ref="#PWR025"  Part="1" 
AR Path="/606BAB07/606B8091" Ref="#PWR07"  Part="1" 
AR Path="/606BABA5/606B8091" Ref="#PWR010"  Part="1" 
AR Path="/606BAC1B/606B8091" Ref="#PWR013"  Part="1" 
AR Path="/606BAC89/606B8091" Ref="#PWR016"  Part="1" 
AR Path="/606BACDB/606B8091" Ref="#PWR019"  Part="1" 
AR Path="/606BAD2D/606B8091" Ref="#PWR022"  Part="1" 
F 0 "#PWR025" H 5350 2700 50  0001 C CNN
F 1 "+3.3V" H 5365 3023 50  0000 C CNN
F 2 "" H 5350 2850 50  0001 C CNN
F 3 "" H 5350 2850 50  0001 C CNN
	1    5350 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 2850 5350 2975
$Comp
L power:GND #PWR?
U 1 1 606B8098
P 5350 4075
AR Path="/606B8098" Ref="#PWR?"  Part="1" 
AR Path="/606B7288/606B8098" Ref="#PWR06"  Part="1" 
AR Path="/606BA995/606B8098" Ref="#PWR027"  Part="1" 
AR Path="/606BAB07/606B8098" Ref="#PWR09"  Part="1" 
AR Path="/606BABA5/606B8098" Ref="#PWR012"  Part="1" 
AR Path="/606BAC1B/606B8098" Ref="#PWR015"  Part="1" 
AR Path="/606BAC89/606B8098" Ref="#PWR018"  Part="1" 
AR Path="/606BACDB/606B8098" Ref="#PWR021"  Part="1" 
AR Path="/606BAD2D/606B8098" Ref="#PWR024"  Part="1" 
F 0 "#PWR027" H 5350 3825 50  0001 C CNN
F 1 "GND" H 5355 3902 50  0000 C CNN
F 2 "" H 5350 4075 50  0001 C CNN
F 3 "" H 5350 4075 50  0001 C CNN
	1    5350 4075
	1    0    0    -1  
$EndComp
Text HLabel 6425 3325 2    50   Input ~ 0
CLK
Text HLabel 6425 3425 2    50   Output ~ 0
MISO
Text HLabel 6425 3625 2    50   Input ~ 0
~CS~
$Comp
L lc_Cap:0402_1.0uF__105_10%_25V C1
U 1 1 606B87B3
P 4500 2975
AR Path="/606B7288/606B87B3" Ref="C1"  Part="1" 
AR Path="/606BA995/606B87B3" Ref="C8"  Part="1" 
AR Path="/606BAB07/606B87B3" Ref="C2"  Part="1" 
AR Path="/606BABA5/606B87B3" Ref="C3"  Part="1" 
AR Path="/606BAC1B/606B87B3" Ref="C4"  Part="1" 
AR Path="/606BAC89/606B87B3" Ref="C5"  Part="1" 
AR Path="/606BACDB/606B87B3" Ref="C6"  Part="1" 
AR Path="/606BAD2D/606B87B3" Ref="C7"  Part="1" 
F 0 "C8" H 4850 3222 50  0000 C CNN
F 1 "0402_1.0uF__105_10%_25V" H 4549 3205 50  0001 L BNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 4499 2695 50  0001 L BNN
F 3 "http://www.szlcsc.com/product/details_53938.html" H 4499 2795 50  0001 L BNN
F 4 "贴片电容" H 4500 2975 50  0001 C CNN "description"
F 5 "供应商链接" H 4499 2595 50  0001 L BNN "ComponentLink1Description"
F 6 "0402" H 4499 2495 50  0001 L BNN "Package"
F 7 "LC" H 4499 2395 50  0001 L BNN "Supplier"
F 8 "C52923" H 4499 2295 50  0001 L BNN "SuppliersPartNumber"
F 9 "1.0uF (105) 10% 25V" H 4850 3131 50  0000 C CNN "Comment"
F 10 "C52923" H 4500 2975 50  0001 C CNN "LCSC Part #"
	1    4500 2975
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 2975 5350 2975
Connection ~ 5350 2975
Wire Wire Line
	5350 2975 5350 3125
$Comp
L power:GND #PWR?
U 1 1 606B94FF
P 4525 2975
AR Path="/606B94FF" Ref="#PWR?"  Part="1" 
AR Path="/606B7288/606B94FF" Ref="#PWR05"  Part="1" 
AR Path="/606BA995/606B94FF" Ref="#PWR026"  Part="1" 
AR Path="/606BAB07/606B94FF" Ref="#PWR08"  Part="1" 
AR Path="/606BABA5/606B94FF" Ref="#PWR011"  Part="1" 
AR Path="/606BAC1B/606B94FF" Ref="#PWR014"  Part="1" 
AR Path="/606BAC89/606B94FF" Ref="#PWR017"  Part="1" 
AR Path="/606BACDB/606B94FF" Ref="#PWR020"  Part="1" 
AR Path="/606BAD2D/606B94FF" Ref="#PWR023"  Part="1" 
F 0 "#PWR026" H 4525 2725 50  0001 C CNN
F 1 "GND" H 4530 2802 50  0000 C CNN
F 2 "" H 4525 2975 50  0001 C CNN
F 3 "" H 4525 2975 50  0001 C CNN
	1    4525 2975
	1    0    0    -1  
$EndComp
Wire Wire Line
	4525 2975 4700 2975
Wire Wire Line
	6425 3625 5750 3625
Wire Wire Line
	5750 3425 6425 3425
Wire Wire Line
	6425 3325 5750 3325
$Comp
L Connector:Screw_Terminal_01x02 J?
U 1 1 606B809E
P 4395 3555
AR Path="/606B809E" Ref="J?"  Part="1" 
AR Path="/606B7288/606B809E" Ref="J3"  Part="1" 
AR Path="/606BA995/606B809E" Ref="J10"  Part="1" 
AR Path="/606BAB07/606B809E" Ref="J4"  Part="1" 
AR Path="/606BABA5/606B809E" Ref="J5"  Part="1" 
AR Path="/606BAC1B/606B809E" Ref="J6"  Part="1" 
AR Path="/606BAC89/606B809E" Ref="J7"  Part="1" 
AR Path="/606BACDB/606B809E" Ref="J8"  Part="1" 
AR Path="/606BAD2D/606B809E" Ref="J9"  Part="1" 
F 0 "J10" H 4313 3230 50  0000 C CNN
F 1 "Screw_Terminal_01x02" H 4313 3321 50  0000 C CNN
F 2 "TerminalBlock_MetzConnect:TerminalBlock_MetzConnect_Type059_RT06302HBWC_1x02_P3.50mm_Horizontal" H 4395 3555 50  0001 C CNN
F 3 "~" H 4395 3555 50  0001 C CNN
F 4 "C474892" H 4395 3555 50  0001 C CNN "LCSC Part #"
	1    4395 3555
	-1   0    0    1   
$EndComp
Wire Wire Line
	5350 3925 5350 4075
$Comp
L Sensor_Temperature:MAX31855KASA U?
U 1 1 606B808B
P 5350 3525
AR Path="/606B808B" Ref="U?"  Part="1" 
AR Path="/606B7288/606B808B" Ref="U1"  Part="1" 
AR Path="/606BA995/606B808B" Ref="U8"  Part="1" 
AR Path="/606BAB07/606B808B" Ref="U2"  Part="1" 
AR Path="/606BABA5/606B808B" Ref="U3"  Part="1" 
AR Path="/606BAC1B/606B808B" Ref="U4"  Part="1" 
AR Path="/606BAC89/606B808B" Ref="U5"  Part="1" 
AR Path="/606BACDB/606B808B" Ref="U6"  Part="1" 
AR Path="/606BAD2D/606B808B" Ref="U7"  Part="1" 
F 0 "U8" H 5075 3900 50  0000 C CNN
F 1 "MAX31855KASA+T" H 5750 3900 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 6350 3175 50  0001 C CIN
F 3 "http://datasheets.maximintegrated.com/en/ds/MAX31855.pdf" H 5350 3525 50  0001 C CNN
F 4 "C52028" H 5350 3525 50  0001 C CNN "LCSC Part #"
	1    5350 3525
	1    0    0    -1  
$EndComp
Wire Wire Line
	4595 3455 4950 3455
Wire Wire Line
	4950 3455 4950 3425
Wire Wire Line
	4595 3555 4950 3555
Wire Wire Line
	4950 3555 4950 3625
$EndSCHEMATC
