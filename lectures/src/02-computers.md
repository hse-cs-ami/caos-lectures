# Что же такое компьютер

Мы будем говорить про классические цифровые (а именно — двоичные) электронные программируемые вычислители.

### Про что мы говорить не будем

![Аналоговое устройство для вычислений — логарифмическая линейка](computers/slide-rule.jpeg)

Аналоговое устройство для вычислений — логарифмическая линейка

![Арифмометр «Феликс»](computers/felix.jpg)
Арифмометр «Феликс»

The **Z1** was a motor-driven mechanical [computer](https://en.wikipedia.org/wiki/Computer) designed by [Konrad Zuse](https://en.wikipedia.org/wiki/Konrad_Zuse) from 1936 to 1937, which he built in his parents' home from 1936 to 1938. It was a binary electrically driven mechanical calculator with limited programmability, reading instructions from punched celluloid film.

![Цифровой двоичный программируемый механический вычислитель [Z1](https://en.wikipedia.org/wiki/Z1_(computer)) (модель)](computers/640px-German_Museum_of_Technology_Berlin_2017_024.jpg)

Цифровой двоичный программируемый механический вычислитель [Z1](https://en.wikipedia.org/wiki/Z1_(computer)) (модель)

**ENIAC** ([/ˈɛniæk/](https://en.wikipedia.org/wiki/Help:IPA/English); **Electronic Numerical Integrator and Computer**) was the first [programmable](https://en.wikipedia.org/wiki/Computer_programming), [electronic](https://en.wikipedia.org/wiki/Electronics), general-purpose [digital computer](https://en.wikipedia.org/wiki/Digital_computer), completed in 1945. Although ENIAC was designed and primarily used to calculate [artillery](https://en.wikipedia.org/wiki/Artillery) [firing tables](https://en.wikipedia.org/wiki/External_ballistics) for the [United States Army](https://en.wikipedia.org/wiki/United_States_Army)'s [Ballistic Research Laboratory](https://en.wikipedia.org/wiki/Ballistic_Research_Laboratory), its first program was a study of the feasibility of the [thermonuclear weapon](https://en.wikipedia.org/wiki/Thermonuclear_weapon).

![Бетти Джин Дженнигс и Рут Байлас программируют электронный компьютер ENIAC](computers/Two_women_operating_ENIAC_(full_resolution).jpg)

Бетти Джин Дженнигс и Рут Байлас программируют электронный компьютер ENIAC

![Квантовый компьютер IBM Q](computers/kvantovye-vychisleniya-otjig-s-vyklyuchatelyami-i-prochee-vesele-17.png)

Квантовый компьютер IBM Q

### Про что мы говорить будем: архитектура фон Неймана

[https://skillbox.ru/media/code/printsipy-fon-neymana-i-pervye-kompyutery-na-ikh-osnove/](https://skillbox.ru/media/code/printsipy-fon-neymana-i-pervye-kompyutery-na-ikh-osnove/)

![Архитектура фон Неймана](computers/von-neumann.png)

Фон-Неймановский компьютер:
* электронный, цифровой;
* адресуемая память — набор одинаковых пронумерованных ячеек;
* хранимая программа в виде набора инструкций;
* инструкции исполняются по одной, в определённой последовательности; есть IP — instruction pointer, ячейка памяти внутри control unit, где хранится адрес текущей инструкции;
* инструкции и данные хранятся в одной и той же памяти, инструкции можно обрабатывать как данные.

Von Neumann bottleneck: одна и та же *шина памяти*
используется для передачи инструкций и данных.

Типичный шаг исполнения программы:
* fetch — из памяти приносим следующую инструкцию;
* decode — определяем, как она будет исполняться;
* execute — исполнение:
  * производим вычисления и обновляем операнды;
  * обновляем instruction pointer.

ISA (instruction set architecture) —
набор инструкций конкретного компьютера, их семантика
и способ кодирования в памяти.
Примеры ISA: x86, ARM, MIPS, RISC-V.

Компьютеры с одинаковой ISA могут быть по-разному
устроены на уровне *микроархитектуры* — от этого
может зависеть скорость или эффективность исполнения программ.

См. также: гарвардская архитектура (инструкции и данные отдельно): [https://en.wikipedia.org/wiki/Harvard_architecture](https://en.wikipedia.org/wiki/Harvard_architecture)
