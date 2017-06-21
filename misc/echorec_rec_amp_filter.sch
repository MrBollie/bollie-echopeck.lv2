<Qucs Schematic 0.0.19>
<Properties>
  <View=0,0,800,800,1,0,0>
  <Grid=10,10,1>
  <DataSet=echorec_rec_amp_filter.dat>
  <DataDisplay=echorec_rec_amp_filter.dpl>
  <OpenDisplay=1>
  <Script=echorec_rec_amp_filter.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
</Symbol>
<Components>
  <R R1 1 160 180 -26 15 0 0 "330 kOhm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C1 1 160 140 -26 17 0 0 "150 pF" 1 "" 0 "neutral" 0>
  <.AC AC1 1 350 70 0 37 0 0 "lin" 1 "1 Hz" 1 "24 kHz" 1 "100" 1 "no" 0>
  <GND * 1 240 420 0 0 0 0>
  <GND * 1 470 320 0 0 0 0>
  <C C2 1 200 390 17 -26 1 3 "10 nF" 1 "" 0 "neutral" 0>
  <R R3 1 240 390 15 -26 0 1 "560 kOhm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Eqn v2db 1 570 90 -31 15 0 0 "x=Pr1.v" 1 "y=20*log10(x/1)" 1 "yes" 0>
  <Vac V1 1 80 270 18 -26 0 1 "1 V" 1 "1 GHz" 0 "0" 0 "0" 0>
  <R R2 1 240 330 15 -26 0 1 "120 kOhm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <VProbe Pr1 1 460 260 28 -31 0 0>
</Components>
<Wires>
  <130 140 130 180 "" 0 0 0 "">
  <190 140 190 180 "" 0 0 0 "">
  <200 360 240 360 "" 0 0 0 "">
  <80 180 130 180 "" 0 0 0 "">
  <80 180 80 240 "" 0 0 0 "">
  <80 300 80 420 "" 0 0 0 "">
  <80 420 200 420 "" 0 0 0 "">
  <200 420 240 420 "" 0 0 0 "">
  <190 180 240 180 "" 0 0 0 "">
  <240 180 240 280 "" 0 0 0 "">
  <470 280 470 320 "" 0 0 0 "">
  <240 280 240 300 "" 0 0 0 "">
  <240 280 450 280 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
