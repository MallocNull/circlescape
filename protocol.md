# PROTOCOL DEFINITION

Messages communicated between the client and server follow the same format, but have different meanings depending on which end is the recipient. A message's intent is determined by its packet ID, a unique identifier that tells the client or server how it should react to the received message. A message id that incites bidirectional communication between the client and server should typically be associated with the same message id on the client as on the server, so as to avoid confusion.

A packet of communication between the client and server is considered to be a seamlessly connected regions of bytes, the boundaries of which are defined in the header of the packet.

All references to the 'byte' in this document refers to individual 8-bit octets, as is the de facto standard in modern computing. All binary-encoded multi-byte quantities sent in a packet are to be sent in network byte order (big endian).

## Header

Because the body of the packet is a sequence of many different regions of byte data that is not delimited, it is necessary for the header of the packet to determine boundaries for the regions of data.

* The first byte is the packet id, the meanings of which are defined in the [_Packet IDs_](#packet-ids) section.
* The second byte is the number of byte regions in the packet.
* The bytes following the second byte are a list of binary length segments, each of which correspond to the number of bytes in its respective region. They each follow this format:
    * If length is less than 254, the length of the region is stored in a single byte.
    * If length is greater than or equal to 254 but less than 65,536, the first byte of the lenght segment should be 254 and the following two bytes is the length of the region.
    * If length is greater than or equal to 65,536, the first byte of the length segment should be 255 and the following four bytes is the length of the region.
    
The number of length segments should be equal to the number of byte regions as defined in the second byte. The length of any single section may not exceed (2^32)-1 bytes.

## Body

The message body immediately follows the header with no separator, and consists of a sequence of byte regions as defined in the header that are joined together without any separator. The position of a byte region in the body should correspond to the offset of the length segment in the header.

### Numeric Packing

All numbers, unless otherwise specified, are the string representation of a base 10 number. Common exceptions are listed below:

* User IDs: 8 bytes, integer, unsigned
* Co-ordinates:  8 bytes, double-precision float

### Packet IDs

A packet ID may have a specific "direction" of communication, in that an endpoint may either act as a _requester_ or a _responder_. A _requester_ is an endpoint that drives all of the communication on that specific packet ID, while the _responder_ is responsible for providing a timely response to the requests. A _responder_ for a specific packet ID should never send that packet ID unsolicited; either the packet will be ignored or the other endpoint will close the connect. Any packet ID marked as bidirectional may be initiated by either endpoint at any time.

#### Server to Client

<table>
 <thead>
  <th colspan="100">
   <center>
    ID 0: Key Exchange<br />
    Requester
   </center>
  </th>
 </thead>
 <thead>
  <th colspan="2">#</th>
  <th>Region</th>
  <th>Type</th>
  <th>Enum</th>
 </thead>
 <tr style="text-align: center;">
  <td colspan="2">1</td>
  <td>Step</td>
  <td>Byte</td>
  <td><i>s &isin; [0, 2]</i></td>
 </tr>
 <tr style="text-align: center;">
  <td><i>s = 0</i></td>
  <td>2</td>
  <td>Generator</td>
  <td>Byte</td>
  <td></td>
 </tr>
</table>

#### Client to Server

<table>
 <thead>
  <th colspan="100">
   <center>
    ID 0: Key Exchange<br />
    Responder
   </center>
  </th>
 </thead>
 <thead>
  <th>#</th>
  <th>Region</th>
  <th></th>
  <th></th>
  <th></th>
  <th>Type</th>
 </thead>
 <tr>
  <td>1</td>
 </tr>
</table>

## Sockstamps

Because epoch time is not standardized across systems, an intermediate layer of date/time transmission must be used between the client and server so as to handle time dependent interactions. Therefore, a "sockstamp" will be used in place of the context-dependent implementations of epoch time.

A sockstamp is a sequence of six bytes that represent a fully qualified date and time on the Gregorian calendar. For the best use of space without obfuscating the data too much, the year's lower four bits and the four bits signifying the month are shared in the same byte, but no other components are joined.

The 12 bits signifying the year are an unsigned quanitity, and indicate the number of years since 0 AD; any date prior to the year of Christ's birth cannot be represented in this format, but this should never be necessary. The effective range of years that can be expressed by this format is 1 AD to 4095 AD. Because the year 0 AD is not a legal year in the Gregorian calendar, this value should never be zero.

The indexed list below indicates which byte (first byte being the MSB) contains what information:
1. Upper 8 bits of the year quantifier.
2. Upper 4 bits are the four least significant bits of the 12-bit year quantifier. The lower 4 bits are the month quantifier, ranging from 0 to 11.
3. Day of month quanitifier. Ranges from 0 to 30.
4. Hour quantifier. Ranges from 0 to 23.
5. Minute quantifier. Ranges from 0 to 59.
6. Second quantifier. Ranges from 0 to 59.

In the event that an endpoint cannot evaluate a date required by the protocol as a result of some error, an error sockstamp will be sent in its place. An error sockstamp takes the form of zeroes in all bits. If an endpoint receives a sockstamp where the year quantifier is zero but any other quantifiers are nonzero, there is a communication error and the endpoint must close the connection.