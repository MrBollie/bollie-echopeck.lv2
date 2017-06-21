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
  <R R2 1 240 290 15 -26 0 1 "120 kOhm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R3 1 240 390 15 -26 0 1 "550 kOhm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <C C2 1 200 390 17 -26 0 1 "10 nF" 1 "" 0 "neutral" 0>
  <GND * 1 240 450 0 0 0 0>
  <VProbe Pr1 1 460 300 28 -31 0 0>
  <.AC AC1 1 350 70 0 37 0 0 "lin" 1 "1 Hz" 1 "24 kHz" 1 "100" 1 "no" 0>
  <Vac V1 1 80 270 18 -26 0 1 "800 mV" 1 "1 GHz" 0 "0" 0 "0" 0>
</Components>
<Wires>
  <130 140 130 180 "" 0 0 0 "">
  <190 140 190 180 "" 0 0 0 "">
  <240 320 240 360 "" 0 0 0 "">
  <200 360 240 360 "" 0 0 0 "">
  <200 420 240 420 "" 0 0 0 "">
  <80 180 130 180 "" 0 0 0 "">
  <240 420 240 450 "" 0 0 0 "">
  <80 180 80 240 "" 0 0 0 "">
  <80 300 80 450 "" 0 0 0 "">
  <80 450 80 460 "" 0 0 0 "">
  <80 450 240 450 "" 0 0 0 "">
  <240 320 450 320 "output" 350 290 79 "">
  <470 320 470 450 "" 0 0 0 "">
  <240 450 470 450 "" 0 0 0 "">
  <190 180 240 180 "" 0 0 0 "">
  <240 180 240 260 "rc1" 270 210 61 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
</Paintings>
