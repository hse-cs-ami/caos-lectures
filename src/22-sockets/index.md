## Сети и сокеты

Для обмена информацией в компьютерных сетях придуманы стандартные *протоколы*
(договорённости о том, кто когда что кому отправляет и что это значит).

### TCP/IP
 [Стек протоколов Интернета](https://en.wikipedia.org/wiki/Internet_protocol_suite),
или стек TCP/IP, выглядит примерно так:

| Уровень | Протоколы этого уровня |
| --- | --- |
| Прикладной (application) | HTTP, SSH, DNS, ... |
| Транспортный (transport) | TCP, UDP, SCTP, QUIC, ... |
| Межсетевой (inter-net) | IPv4, IPv6 |
| Канальный (link) | Ethernet, Wi-Fi (IEEE 802.11), ... |

Обычно мы используем набор протоколов разных уровней, чтобы решить свою
прикладную задачу. Например, при загрузке веб-страницы
[http://wiki.cs.hse.ru/](http://wiki.cs.hse.ru/) будут как минимум
использоваться HTTP, TCP, IPv4 и какие-то канальные протоколы.

Фрагмент данных с верхнего уровня стека заворачивается в “конверт” нижнего
уровня (картинка из википедии на примере UDP):

![Untitled](sockets/Untitled.png)

Данные доходят до адресата через цепочку промежуточных устройств, каждое из
которых распаковывает и просматривает столько конвертов, сколько ему нужно и
сколько оно умеет:

![Untitled](sockets/Untitled%201.png)

Например, пока пакет не дошёл до *хоста* (компьютера) – адресата, никто* не
смотрит на заголовки транспортного уровня. Благодаря этому поверх имеющейся
инфраструктуры межсетевого уровня можно реализовывать новые способы обмена
информацией.

Как правило, протоколы прикладного уровня реализованы в userspace (в программах
и библиотеках), транспортного и межсетевого — в ядре ОС, а канальный уровень
делят между собой ОС и аппаратура.

На межсетевом уровне появляется глобальная адресация: у каждого хоста в сети
есть уникальный идентификатор — *адрес*. Адрес IPv4 — 4 байта, их записывают в
десятичной системе: `92.242.58.220`. Адрес IPv6 — 16 байт, их записывают в
16-ричной системе двухбайтовыми последовательностями через двоеточие:
`2a02:6b8::2:242` (там, где два двоеточия подряд, подразумевается
последовательность нулевых байт).

[Статья 2003 года про сложности с переходом на IPv6](https://cr.yp.to/djbdns/ipv6mess.html)

Протокол TCP представляет абстракцию “трубы с данными”, похожей на канал в Unix:
гарантируется надёжная доставка данных в изначальной последовательности, не
сохраняются границы между отдельными записями в канал. Протокол UDP представляет
абстракцию “голубиной почты”: можно отправлять *датаграммы* — отдельные
фрагменты данных известного размера, которые могут дойти до получателя в
произвольном порядке, дойти несколько раз или не дойти вовсе.

TCP и UDP используют адрес межсетевого уровня и добавляют ещё 16 бит, которые
называются «*порт*». Есть well-known порты, которые обычно используются для
разных надобностей (см. `/etc/services`): например, сервер HTTP обычно
использует порт 80.

### ISO/OSI
[Википедия](https://en.wikipedia.org/wiki/OSI_model)

Семиуровневая модель, реализации которой не получили распространения,
но уровни которой всё равно приходится знать:

1. Physical
2. Data Link
3. Network
4. Transport
5. Session
6. Presentation
7. Application

### Ну и теперь собственно сокеты

POSIX даёт нам слой абстракции поверх протоколов транспортного уровня и ниже.
Абстракция называется [sockets](https://en.wikipedia.org/wiki/Berkeley_sockets)
(гнёзда). В userspace сокет виден как файловый дескриптор.

Создаём сокет ([man 2 socket](https://man7.org/linux/man-pages/man2/socket.2.html)):

```c
#include <sys/types.h>
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
```

Параметр domain выбирает используемый протокол межсетевого уровня (ну примерно).
Нам будут интересны три варианта:

```
Name         Purpose                                    Man page
AF_INET      IPv4 Internet protocols                    ip(7)
AF_INET6     IPv6 Internet protocols                    ipv6(7)
AF_UNIX      Local communication                        unix(7)
```

Сокеты бывают как минимум двух типов (параметр type): потоковые и датаграммные
(ср. TCP и UDP). Для первых хорошо подходят стандартные операции read/write, для
вторых не очень.

```
       SOCK_STREAM     Provides sequenced, reliable, two-way, connection-based
                       byte  streams.

       SOCK_DGRAM      Supports datagrams (connectionless, unreliable messages
                       of a fixed maximum length).
```

protocol мы будем обычно указывать равным нулю, чтобы ОС выбрала за нас
стандартный протокол нужного типа (TCP для AF_INET/SOCK_STREAM, UDP для
AF_INET/SOCK_DGRAM), но можно и явно указать IPPROTO_TCP или IPPROTO_UDP.

### Где бы взять адрес

Допустим, мы хотим соединиться с хостом [ya.ru](http://ya.ru) по протоколу
HTTPS. Мы пока не знаем, есть ли у хоста адрес IPv6 или IPv4 и каковы эти
адреса. Нам нужно обратиться к серверу DNS (Domain Name System) и спросить у
него. Ещё неплохо бы посмотреть в /etc/services, какого типа сокет нам
понадобится (stream или datagram) и какой использовать порт.

Для этого нам дана функция getaddrinfo:

```c
       int getaddrinfo(const char *node, const char *service,
                       const struct addrinfo *hints,
                       struct addrinfo **res);

       void freeaddrinfo(struct addrinfo *res);

       const char *gai_strerror(int errcode);

       struct addrinfo {
               int              ai_flags;
               int              ai_family;
               int              ai_socktype;
               int              ai_protocol;
               socklen_t        ai_addrlen;
               struct sockaddr *ai_addr;
               char            *ai_canonname;
               struct addrinfo *ai_next;
       };
```

Попробуем распечатать адреса, которые она нам возвращает:

```c
{{#rustdoc_include code/gai.c}}
```

Теперь у нас есть адреса, ~~по которым найдём сервиса́~~ и можно создавать
сокеты подходящих типов и пробовать устанавливать соединения.

### Потоковые сокеты

Есть две стороны: клиент (активная сторона) и сервер (пассивная сторона).

Клиент:

```c
{{#rustdoc_include code/client.c}}
```

Сервер, принимающий одно соединение:

```c
{{#rustdoc_include code/server.c}}
```

### Датаграммные сокеты

```c
       ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
                      const struct sockaddr *dest_addr, socklen_t addrlen);

       ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                        struct sockaddr *src_addr, socklen_t *addrlen);
```

### Unix domain sockets

```c
{{#rustdoc_include code/unix_server.c}}
```


### Ещё об адресах

[Структуры sockaddr_*](https://man.archlinux.org/man/sockaddr_storage.3type.en)

[Сетевой порядок байт](https://man.archlinux.org/man/byteorder.3.en)
