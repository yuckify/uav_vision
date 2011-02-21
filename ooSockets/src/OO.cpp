#include"OO.hpp"

ostream& operator<<(ostream& out, OO::SockType t) {
	switch(t) {
		case OO::Udp:		out<<"Udp";	break;
		case OO::Tcp:		out<<"Tcp";	break;
		case OO::Raw:		out<<"Raw";	break;
		case OO::NullType:	out<<"NullType"; break;
	}

	return out;
}

ostream& operator<<(ostream& out, OO::SockFamily f) {
	switch(f) {
		case OO::Inet4:			out<<"Inet4";		break;
		case OO::Inet6:			out<<"Inet6";		break;
		case OO::NullFamily:	out<<"NullFamily";	break;
		case OO::Unix:			out<<"Unix";		break;
	}

	return out;
}

ostream& operator<<(ostream& out, OO::SockFlags f) {
	int outs = 0;
	if(f & OO::Passive) {
		outs++;
		out<<"Passive";
	}
	if(f & OO::NumericHost) {
		if(outs) out<<" ";
		outs++;
		out<<"NumericHost";
	}
	if(f & OO::NumericServ) {
		if(outs) out<<" ";
		outs++;
		out<<"NumericServ";
	}
	if(f & OO::V4Mapped) {
		if(outs) out<<" ";
		outs++;
		out<<"V4Mapped";
	}
	if(f & OO::All) {
		if(outs) out<<" ";
		outs++;
		out<<"All";
	}
	if(f & OO::AddrConfig) {
		if(outs) out<<" ";
		outs++;
		out<<"AddrConfig";
	}
	
	return out;
}

ostream& operator<<(ostream& out, OO::SockProto p) {
	switch(p) {
		case OO::ProtoIcmp: out<<"ProtoIcmp";	break;
		case OO::ProtoIpv4:	out<<"ProtoIpv4";	break;
		case OO::ProtoIpv6:	out<<"ProtoIpv6";	break;
		case OO::ProtoRaw:	out<<"ProtoRaw";	break;
		case OO::ProtoTcp:	out<<"ProtoTcp";	break;
		case OO::ProtoUdp:	out<<"ProtoUdp";	break;
	}
	
	return out;
}

