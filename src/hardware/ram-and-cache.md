# Память и кэш

## Устройство ОЗУ
Мы уже видели SRAM — память на триггерах.

Основную часть памяти компьютера составляет DRAM:
![](../dram.png)

Процессор и память общаются посредством *шины* (bus):
![](../yabloko/io/system_bus.png)

Row size: ≈2KB


## Кэш

[Про кэш на uneex](https://www.uneex.ru/LecturesCMC/ArchitectureAssembler2024/11_CacheBPT#line-46)

[Симулятор кэша](https://claude.site/artifacts/5ab29b0e-1ca7-478e-aa71-a9e09340b2de?fullscreen=true)

Понятия:
* тэг
* строка кэша
* ассоциативность
* кэш прямого доступа (1-way)
* полноассоциативный кэш (N-way)

[Latency numbers every programmer should know](https://gist.github.com/jboner/2841832)