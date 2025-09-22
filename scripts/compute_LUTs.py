# compute and print as C array all the half_periods
print("const uint16_t half_periods[128] PROGMEM = {")
for i in range(16):
	freql = []
	strl = []
	for j in range(8):
		n = i*8+j
		f = (440*(2**((n-69)/12)))
		period_usec = 1000000/f
		half_period_usec = 500000/f
		strl.append(str(round(half_period_usec)).ljust(5," "))

	print("\t", "/TIMER_RESOLUTION, ".join(strl),",")
print("};")
