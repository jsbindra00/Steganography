#include <SFML/Graphics/Image.hpp>
#include <bitset>
#include <algorithm>
#include <fstream>
#include <array>
#include <sstream>
#include <fstream>


using CharBitmask = std::bitset<8>;

std::vector<CharBitmask> stringToBits(const std::string& str)
{
    std::vector<CharBitmask> res;
    std::transform(str.begin(), str.end(), std::back_inserter(res), [](const char ch)
        {
            return CharBitmask(ch);
        });
    return res;
}
char replaceBits(uint8_t ch, const CharBitmask& mask, std::size_t start, std::size_t end)
{
    for (std::size_t replacementBitIndex = start; replacementBitIndex <= end; ++replacementBitIndex)
    {
        const auto bit = mask[replacementBitIndex];
        ch ^= (-bit ^ ch) & (1 << (replacementBitIndex - start));
    }
    return ch;
}
std::array<uint8_t, 4> colorToChannels(sf::Color c)
{
    return { c.r,c.g,c.b,c.a };
}

void encodeImage(sf::Image& img, const std::string& message)
{
    auto messageBytes = stringToBits(message);
    sf::Vector2u rawImageSize = img.getSize();

    std::size_t currentByteIndex = 0;
    for (int j = 0; j < rawImageSize.y; ++j)
    {
        for (int i = 0; i < rawImageSize.x; ++i)
        {
	
            if (currentByteIndex == messageBytes.size()) return;
            sf::Color currentPixel = img.getPixel(i, j);
            std::array<uint8_t, 4> encodedChannels = colorToChannels(currentPixel);
            
			const CharBitmask& mask = messageBytes.at(currentByteIndex);
			std::for_each(encodedChannels.begin(), encodedChannels.end(), [k = 0, &mask](uint8_t& ch) mutable{ch = replaceBits(ch, mask, 2*k, 2*k + 1); ++k;});

			currentPixel.r = encodedChannels[0];
			currentPixel.g = encodedChannels[1];
			currentPixel.b = encodedChannels[2];
			currentPixel.a = encodedChannels[3];

            
            img.setPixel(i, j, currentPixel);
            ++currentByteIndex;
        }   
    }
}


std::string decodeimage(const sf::Image& img)
{
    std::string res;
    sf::Vector2u rawImageSize = img.getSize();
	for (int j = 0; j < rawImageSize.y; ++j)
	{
        for (int i = 0; i < rawImageSize.x; ++i)
        {
            sf::Color currentPixel = img.getPixel(i, j);
            std::array<uint8_t, 4> encodedChannels = colorToChannels(currentPixel);
            CharBitmask channelsByte("0");
            for (std::size_t channelIndex = 0; channelIndex < 4; ++channelIndex)
            {
                channelsByte[channelIndex * 2] = encodedChannels[channelIndex] & 1;
                channelsByte[(channelIndex * 2) + 1] = (encodedChannels[channelIndex] >> 1) & 1;
            }
            res.push_back(static_cast<unsigned char>(channelsByte.to_ulong()));
        }
    }
    return res;
}

int main()
{
    sf::Image image;
    image.loadFromFile("doge.jpg");

	std::ifstream fileStream("macbeth.txt");
	std::stringstream buff;
	buff << fileStream.rdbuf();

	std::string message(buff.str());
	encodeImage(image, message);
	image.saveToFile("doge_encode.png");

}
