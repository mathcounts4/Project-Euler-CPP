#pragma once

std::ostream & operator<<(std::ostream & o, __int128 x) {
    if (x<0)
	return o << "-" << -x;
    std::string ans;
    do {
	ans += '0' + x%10;
    } while (x /= 10);
    std::reverse(ans.begin(),ans.end());
    return o << ans;
}
