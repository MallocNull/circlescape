# PROTOCOL DEFINITION

Messages communicated between the client and server follow the same format, but have different meanings depending on which end is the recipient. A message's intent is determined by its packet ID, a unique identifier that tells the client or server how it should react to the received message. A message id that incites bidirectional communication between the client and server should typically be associated with the same message id on the client as on the server, so as to avoid confusion.

A packet of communication between the client and server is considered to be a header followed by seamlessly connected regions of bytes, the boundaries of which are defined in the header.

All references to the 'byte' in this document refers to individual 8-bit octets, as is the de facto standard in modern computing. All binary-encoded multi-byte quantities sent in a packet are to be sent in network byte order (big endian).

## Header

Because the body of the packet is a sequence of many different regions of byte data that is not delimited, it is necessary for the header of the packet to determine boundaries for the regions of data.

* The first two bytes will always be 0xB0 and 0x0B. If this is not set properly, the endpoint must close the connection.
* The next four bytes are the total length of the entire packet, including the whole header.
* The seventh byte is the packet id, the meanings of which are defined in the packet ID list section.
* The eighth byte is the number of byte regions in the packet.
* The bytes following the eighth byte are a list of binary length segments, each of which correspond to the number of bytes in its respective region. They each follow this format:
    * If length is less than 254, the length of the region is stored in a single byte.
    * If length is greater than or equal to 254 but less than 65,536, the first byte of the length segment will be 254 and the following two bytes is the length of the region.
    * If length is greater than or equal to 65,536, the first byte of the length segment will be 255 and the following four bytes is the length of the region.
    
The number of length segments must equal the number of byte regions as defined in the second byte. The combined length of the regions must not exceed 2<sup>32</sup> - _n_ - 1 where _n_ is the length of the header.

### Packet IDs

A packet ID may have a specific "direction" of communication, in that an endpoint may either act as a _requester_ or a _responder_. A _requester_ is an endpoint that drives all of the communication on that specific packet ID, while the _responder_ is responsible for providing a timely response to the requests it receives. A _responder_ for a specific packet ID should never send that packet ID unsolicited; either the packet will be ignored or the other endpoint will close the connection. Any packet ID marked as bidirectional may be initiated by either endpoint at any time.

A _blind requester_ is an endpoint that sends out a packet of a certain ID and either does not expect a response or expects a response on a different packet ID.

## Body

The message body immediately follows the header with no delimiter, and consists of a sequence of byte regions as defined in the header that are laid out in sequence without any delimiters. The position of a byte region in the body should correspond to the offset of the length segment in the header.

### Numeric Packing

All numbers, unless otherwise specified, are the string representation of a base 10 number. Common exceptions are listed below:

* User IDs: Hex string, 8 bytes unsigned
* Co-ordinates:  8 bytes, double-precision float
* Big Int: Hex string, variable size

## Master/Slave Servers

To keep track of the status of multiple servers from a centralized point that the client may query, each server must be able to communicate with a "master" server that will record and dispense information regarding all servers to clients. All servers that report to the master server will hereby be refered to as "slave" servers. 

Communication between master and slave servers will be done over a TCP connection on a port that is defined by the master server's configuration. The protocol used for this communication is identical to the protocol defined for standard client/server communication; however, the Packet IDs are defined differently.

Communication between the master server and clients will be done over a WebSocket connection on a port that is defined by the master server's configuration. The protocol used for this communication is identical to the protocol defined for standard client/server communication; however, the Packet IDs are defined differently.

## Packet ID List

### Master/Slave Packet IDs

#### Master to Slave [Encrypted]

<table style="margin-right: 8px; margin-bottom: 8px;">
 <thead>
  <th colspan="100" class="center">
   ID 0: Positive ACK<br />
   Responder
  </th>
 </thead>
 <thead>
  <th>#</th>
  <th>Region</th>
  <th>Type</th>
 </thead>
 <tr>
  <td>1</td>
  <td>Request Packet ID</td>
  <td>Byte</td>
 </tr>
</table>

<table style="margin-right: 8px; margin-bottom: 8px;">
 <thead>
  <th colspan="100" class="center">
   ID 1: Negative ACK<br />
   Responder
  </th>
 </thead>
 <thead>
  <th>#</th>
  <th>Region</th>
  <th>Type</th>
 </thead>
 <tr>
  <td>1</td>
  <td>Request Packet ID</td>
  <td>Byte</td>
 </tr>
 <tr>
  <td>2</td>
  <td>Error Code</td>
  <td>Packed Unsigned Short</td>
 </tr>
</table>

#### Slave to Master [Encrypted]

<table style="margin-right: 8px; margin-bottom: 8px;">
 <thead>
  <th colspan="100" class="center">
    ID 0: Authentication<br />
    Requester
  </th>
 </thead>
 <thead>
  <th>#</th>
  <th>Region</th>
  <th>Type</th>
 </thead>
 <tr>
  <td class="center">1</td>
  <td>Server Name</td>
  <td>Text</td>
 </tr>
 <tr>
  <td class="center">2</td>
  <td>Port</td>
  <td>Packed Unsigned Short</td>
 </tr>
 <tr>
  <td class="center">3</td>
  <td>License</td>
  <td>Text</td>
 </tr>
 <tr>
  <td class="center">4</td>
  <td>Secret</td>
  <td>Blob (512b)</td>
 </tr>
</table>

<table style="margin-right: 8px; margin-bottom: 8px;">
 <thead>
  <th colspan="100" class="center">
    ID 1: Status Update<br />
    Blind Requester
  </th>
 </thead>
 <thead>
  <th>#</th>
  <th>Region</th>
  <th>Type</th>
 </thead>
 <tr>
  <td class="center">1</td>
  <td>User Count</td>
  <td>Packed Unsigned Short</td>
 </tr>
 <tr>
  <td class="center">2</td>
  <td>Max Users</td>
  <td>Packed Unsigned Short</td>
 </tr>
</table>

### Master/Client Packet IDs

#### Master to Client [Encrypted]

<table style="margin-right: 8px; margin-bottom: 8px;">
 <thead>
  <th colspan="100" class="center">
    ID 0: Login Response<br />
    Responder
  </th>
 </thead>
 <thead>
  <th>#</th>
  <th>Region</th>
  <th>Type</th>
  <th>if</th>
 </thead>
 <tr>
  <td class="center">1</td>
  <td>Succeeded</td>
  <td>Boolean</td>
  <td></td>
 </tr>
 <tr style="border-bottom: 2px solid;"> 
  <td class="center">2</td>
  <td>Error Code</td>
  <td>Packed Unsigned Short</td>
  <td>&not;R<sub>1</sub></td>
 </tr>
 <tr> 
  <td class="center">2</td>
  <td>Secret</td>
  <td>Bytes (128)</td>
  <td>R<sub>1</sub></td>
 </tr>
 </tr>
</table>

<table style="margin-right: 8px; margin-bottom: 8px;">
 <thead>
  <th colspan="100" class="center">
    ID 1: Registration Response<br />
    Responder
  </th>
 </thead>
 <thead>
  <th>#</th>
  <th>Region</th>
  <th>Type</th>
 </thead>
 <tr>
  <td class="center">1</td>
  <td>Succeeded</td>
  <td>Boolean</td>
 </tr>
 <tr>
  <td class="center">2</td>
  <td>Error Code</td>
  <td>Packed Unsigned Short</td>
 </tr>
</table>

<table style="margin-right: 8px; margin-bottom: 8px;">
 <thead>
  <th colspan="100" class="center">
    ID 2: Server Listing<br />
    Responder
  </th>
 </thead>
 <thead>
  <th>#</th>
  <th>Region</th>
  <th>Type</th>
 </thead>
 <tr>
  <td class="center">1</td>
  <td>Server Count (<i>n</i>)</td>
  <td>Packed Unsigned Short</td>
 </tr>
 <tr>
  <td><i>r</i> > 1</td>
  <td colspan="2">Iterated over <i>n</i> (0 &leq; <i>i</i> &leq; <i>n - 1</i>)</td>
 </tr>
 <tr>
  <td class="center">2 + 3<i>i</i></td>
  <td>Server Id</td>
  <td>Packed Unsigned Short</td>
 </tr>
 <tr>
  <td class="center">3 + 3<i>i</i></td>
  <td>Server Name</td>
  <td>Text</td>
 <tr>
  <td class="center">4 + 3<i>i</i></td>
  <td>User Count</td>
  <td>Packed Unsigned Short</td>
 </tr>
 </tr>
</table>

#### Client to Master [Encrypted]

<table style="margin-right: 8px; margin-bottom: 8px;">
 <thead>
  <th colspan="100" class="center">
    ID 0: Login Request<br />
    Requester
  </th>
 </thead>
 <thead>
  <th>#</th>
  <th>Region</th>
  <th>Type</th>
 </thead>
 <tr>
  <td class="center">1</td>
  <td>Username</td>
  <td>String</td>
 </tr>
 <tr>
  <td class="center">2</td>
  <td>Password</td>
  <td>String</td>
 </tr>
</table>

<table style="margin-right: 8px; margin-bottom: 8px;">
 <thead>
  <th colspan="100" class="center">
    ID 1: Registration Request<br />
    Requester
  </th>
 </thead>
 <thead>
  <th>#</th>
  <th>Region</th>
  <th>Type</th>
 </thead>
 <tr>
  <td class="center">1</td>
  <td>Username</td>
  <td>String</td>
 </tr>
 <tr>
  <td class="center">2</td>
  <td>Password</td>
  <td>String</td>
 </tr>
 <tr>
  <td class="center">3</td>
  <td>Email</td>
  <td>String</td>
 </tr>
</table>

<table style="margin-right: 8px; margin-bottom: 8px;">
 <thead>
  <th colspan="100" class="center">
    ID 2: Server List Request<br />
    Requester<br />
  </th>
  <thead>
	<th colspan="100" class="center">
		<i>Bodyless Packet</i>
	</th>
  </thead>
 </thead>
</table>

### Slave/Client Packet IDs

#### Slave to Client

TODO: populate

#### Client to Slave

TODO: populate

## Sockstamps

Because epoch time is not standardized across systems, an intermediate layer of date/time transmission must be used between the client and server so as to handle time dependent interactions. Therefore, a "sockstamp" will be used in place of the context-dependent implementations of epoch time.

A sockstamp is a sequence of six bytes that represent a fully qualified UTC date and time on the Gregorian calendar. For the best use of space without obfuscating the data too much, the year's lower four bits and the four bits signifying the month are shared in the same byte, but no other components are joined.

The 12 bits signifying the year are an unsigned quanitity, and indicate the number of years since 0 AD; any date prior to the year of Christ's birth cannot be represented in this format, but this should never be necessary. The effective range of years that can be expressed by this format is 1 AD to 4095 AD. Because the year 0 AD is not a legal year in the Gregorian calendar, this value should never be zero.

The indexed list below indicates which byte (first byte being the MSB) contains what information:
1. Upper 8 bits of the year quantifier.
2. Upper 4 bits are the four least significant bits of the 12-bit year quantifier. The lower 4 bits are the month quantifier, ranging from 0 to 11.
3. Day of month quanitifier. Ranges from 0 to 30.
4. Hour quantifier. Ranges from 0 to 23.
5. Minute quantifier. Ranges from 0 to 59.
6. Second quantifier. Ranges from 0 to 59.

In the event that an endpoint cannot evaluate a date required by the protocol as a result of some error, an error sockstamp will be sent in its place. An error sockstamp takes the form of zeroes in all bits. If an endpoint receives a sockstamp where the year quantifier is zero but any other quantifiers are nonzero, there is a communication error and the endpoint must close the connection.

## Status Code Index

TODO: MAKE THIS SECTION NOT LOOK LIKE SHIT

### Master / Slave

#### M -> S (ID 1)
0x100: MAX AUTH ATTEMPTS REACHED

0x101: LICENSE DATA INCORRECT

0x102: LICENSE LIMIT REACHED

0x200: NOT AUTHORIZED TO PERFORM ACTION

### Master / Client

#### M -> C (ID 0)

0x100: MAX AUTH ATTEMPTS REACHED

0x101: USERNAME DOES NOT EXIST

0x102: PASSWORD INCORRECT

#### M -> C (ID 1)

0x000: OK

0x100: USERNAME TAKEN

0x101: USERNAME ILLEGAL

0x110: EMAIL TAKEN

0x111: EMAIL ILLEGAL

0x120: PASSWORD TOO WEAK

### Slave / Client