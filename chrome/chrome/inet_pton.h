//这是一个不确定的函数
#define INADDRSZ         4

static int
inet_pton4(const char *src, unsigned char *dst)
{
	static const char digits[] = "0123456789";
	int saw_digit, octets, ch;
	unsigned char tmp[INADDRSZ], *tp;

	saw_digit = 0;
	octets = 0;
	tp = tmp;
	*tp = 0;
	while((ch = *src++) != '\0') {
		const char *pch;

		if((pch = strchr(digits, ch)) != NULL) {
			unsigned int val = *tp * 10 + (unsigned int)(pch - digits);

			if(saw_digit && *tp == 0)
				return (0);
			if(val > 255)
				return (0);
			*tp = (unsigned char)val;
			if(! saw_digit) {
				if(++octets > 4)
					return (0);
				saw_digit = 1;
			}
		}
		else if(ch == '.' && saw_digit) {
			if(octets == 4)
				return (0);
			*++tp = 0;
			saw_digit = 0;
		}
		else
			return (0);
	}
	if(octets < 4)
		return (0);
	memcpy(dst, tmp, INADDRSZ);
	return (1);
}

int
Curl_inet_pton(int af, const char *src, void *dst)
{
	switch (af) {
  case AF_INET:
	  return (inet_pton4(src, (unsigned char *)dst));
#ifdef ENABLE_IPV6
  case AF_INET6:
	  //  return (inet_pton6(src, (unsigned char *)dst));
	  ;
#endif
  default:
	  // SET_ERRNO(EAFNOSUPPORT);
	  return (-1);
	}
	/* NOTREACHED */
}

