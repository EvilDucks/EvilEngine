namespace SIZED_BUFFOR {

	auto GetCount (
		u8* sizedBuffor, 
		const u8& agregatorIndex,
		const u8& offset
	) {
		return sizedBuffor + 1 + offset + agregatorIndex;
	}

	auto GetElement (
		u8* sizedBuffor, 
		const u8& agregatorIndex,
		const u8& elementIndex,
		const u8& offset
	) {
		return sizedBuffor + 2 + offset + agregatorIndex + elementIndex;
	}

}