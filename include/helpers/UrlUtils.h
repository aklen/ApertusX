#ifndef URL_UTILS_H
#define URL_UTILS_H

#include <string>
#include <memory>

#ifdef USE_LIBCURL
#include <curl/curl.h>
#endif

class UrlUtils {
public:
    /**
     * Encode a string for a valid URI component.
     */
    static std::string EncodeUriComponent(const std::string& input) {
#ifdef USE_LIBCURL
        CURL* curl = curl_easy_init();
        if (!curl) return "";
        char* encoded = curl_easy_escape(curl, input.c_str(), static_cast<int>(input.length()));
        std::string result(encoded ? encoded : "");
        curl_free(encoded);
        curl_easy_cleanup(curl);

        // Ensure slashes ("/") remain unencoded
        std::regex slashEncoded("%2F");
        result = std::regex_replace(result, slashEncoded, "/");

        return result;
#else
        return FallbackEncode(input);
#endif
    }

    /**
     * Decode a percent-encoded URI component.
     */
    static std::string DecodeUriComponent(const std::string& input) {
#ifdef USE_LIBCURL
        CURL* curl = curl_easy_init();
        if (!curl) return "";
        int output_length;
        char* decoded = curl_easy_unescape(curl, input.c_str(), static_cast<int>(input.length()), &output_length);
        std::string result(decoded ? std::string(decoded, output_length) : "");
        curl_free(decoded);
        curl_easy_cleanup(curl);
        return result;
#else
        return FallbackDecode(input);
#endif
    }

    /**
     * Convert a file path to a valid file:// URI.
     */
    static std::string ToFileUri(const std::string& path) {
        std::string filePath = path;
        
        // Remove "file://" prefix if it exists
        if (filePath.find("file://") == 0) {
            filePath = filePath.substr(7);
        }

        // Ensure the path is absolute
        if (filePath.empty() || filePath[0] != '/') {
            return "";  // Invalid absolute path
        }

        // Encode only spaces and special characters (NOT slashes)
        std::string encodedPath = EncodeUriComponent(filePath);
        
        return "file://" + encodedPath;
    }

private:
    /**
     * Simple fallback for URL encoding (minimal replacement).
     */
    static std::string FallbackEncode(const std::string& input) {
        std::ostringstream encoded;
        for (unsigned char c : input) {
            if (isalnum(c) || c == '/' || c == '-' || c == '_' || c == '.' || c == '~') {
                encoded << c;
            } else if (c == ' ') {
                encoded << "%20";
            } else {
                encoded << '%' << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(c);
            }
        }
        return encoded.str();
    }

    /**
     * Simple fallback for URL decoding.
     */
    static std::string FallbackDecode(const std::string& input) {
        std::string output;
        size_t i = 0;
        while (i < input.length()) {
            if (input[i] == '%' && i + 2 < input.length()) {
                output += FromHex(input.substr(i + 1, 2));
                i += 3;
            } else {
                output += input[i++];
            }
        }
        return output;
    }

    static std::string ToHex(unsigned char c) {
        const char* hexChars = "0123456789ABCDEF";
        return std::string(1, hexChars[c >> 4]) + std::string(1, hexChars[c & 0x0F]);
    }

    static char FromHex(const std::string& hex) {
        return static_cast<char>(std::stoi(hex, nullptr, 16));
    }
};

#endif // URL_UTILS_H
