# pwr_so2
SO2 P PWr dr inż. Dariusz Caban.  
## Spis zadań
1. Kolejne pojazdy wjeżdżają na tor z losowym opóźnieniem i wykonują 3 okrążenia. Prędkość początkowa losowa, zmienia się po przejechaniu każdego odcinka toru.
2. Jak samochód dogoni samochód przed nim jadący, to powinien czekać z wyprzedzeniem do momentu aż znajdzie się w obszarze, gdzie jest dopuszczalne wyprzedzanie, czyli na każdej stronie / na każdej ścianie ocznaczony jest pewien odcinek, w którym jest dopuszczalne wyprzedzanie i tylko tam będą się samochody wyprzedzały, a inaczej będą się trzymać jeden za drugim.
3. Jak na wjeździe do tych sekcji, gdzie jest wyprzedzanie możliwe, będzie więcej niż 3 czy 4 samochody (specyficzne dla każdej sekcji), to samochody będą się ustawiały w kolejce jeden za drugim. I w momencie jak samochod będzie wyjeżdżał z tej sekcji krytycznej, to zostanie wpuszczony następny pojazd z kolei.
***
Simulation of cars driving on a road.  
Each car is being controlled via different thread. Car leaves the road after completing 3 loops.  
Cars are only allowed to overtake others on selected areas.

