
function byteCountToText(count) {
	if (count < 1024)
		return count + " B"
	else if (count < 1024 * 1024)
		return (count / 1024) + " KB"
	else
		return (count / 1024 / 1024) + " MB"
}
