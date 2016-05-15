#include "converter.h"
#include <iostream>

// konstansok
const std::string SNT_OPEN("🀰");
const std::string SNT_CLOSE("🀱");
const std::string WS_OPEN("🀲");
const std::string WS_CLOSE("🀳");
const std::string WORD_OPEN("🀴");
const std::string WORD_CLOSE("🀵");
const std::string PUNCT_OPEN("🀶");
const std::string PUNCT_CLOSE("🀷");

// konstruktor
Converter::Converter(OUTPUT_TYPE type) {
    switch(type) {
        case XML:
            myConversionMap = xml_map;
            break;
        case JSON:
            myConversionMap = json_map;
            break;
        case TSV:
            myConversionMap = tsv_map;
            break;
    }
}


// atvett szoveg konvertalasa es kiirasa
// TODO: valamiert nem megy referenciaval, pedig az hatekonyabb lenne
void Converter::operator<<(std::string text) {
    convert_tags(text);
    std::cout << text;
}


// vesz egy szoveget, egy 'valamit' es egy 'valamire'-t, majd a szovegben a
// 'valami' osszes elofordulasat lecsereli 'valamire'
void Converter::replace_tag(std::string &text, const std::string &what, const std::string &for_what) {
    for(size_t pos = text.find(what);
        pos != std::string::npos;
        pos = text.find(what, pos+1)) {
        text.replace(pos, what.size(), for_what);
    }
}


// vesz egy szotarat es egy szoveget, a szovegben lecsereli a szotar minden
// tetelenek minden elofordulasat a szotarban megadott megfelelojere
std::string& Converter::convert_tags(std::string &text) {
    for(auto it = myConversionMap.begin(); it != myConversionMap.end(); ++it) {
        replace_tag(text, it->first, it->second);
    }
    return text;
}


// szotarak feltoltese
// xml:
Converter::ConversionMap Converter::createXmlMap() {
    ConversionMap xml_map;
    xml_map.push_back(std::make_pair(SNT_OPEN, "<s>"));
    xml_map.push_back(std::make_pair(SNT_CLOSE, "</s>"));
    xml_map.push_back(std::make_pair(WS_OPEN, "<ws>"));
    xml_map.push_back(std::make_pair(WS_CLOSE, "</ws>"));
    xml_map.push_back(std::make_pair(WORD_OPEN, "<w>"));
    xml_map.push_back(std::make_pair(WORD_CLOSE, "</w>"));
    xml_map.push_back(std::make_pair(PUNCT_OPEN, "<c>"));
    xml_map.push_back(std::make_pair(PUNCT_CLOSE, "</c>"));
    return xml_map;
}

// json:
Converter::ConversionMap Converter::createJsonMap() {
    ConversionMap json_map;
    // close json items with comma and open another
    json_map.push_back(std::make_pair(SNT_CLOSE+WS_OPEN, "],\n\"ws\": \""));
    json_map.push_back(std::make_pair(SNT_CLOSE+SNT_OPEN, "],\n\"s\": [\n"));
    json_map.push_back(std::make_pair(WS_CLOSE+SNT_OPEN, "\",\n\"s\": [\n"));
    json_map.push_back(std::make_pair(WS_CLOSE+WORD_OPEN, "\",\n\"w\": \""));
    json_map.push_back(std::make_pair(WS_CLOSE+PUNCT_OPEN, "\",\n\"c\": \""));
    json_map.push_back(std::make_pair(WORD_CLOSE+WS_OPEN, "\",\n\"ws\": \""));
    json_map.push_back(std::make_pair(WORD_CLOSE+WORD_OPEN, "\",\n\"w\": \""));
    json_map.push_back(std::make_pair(WORD_CLOSE+PUNCT_OPEN, "\",\n\"c\": \""));
    json_map.push_back(std::make_pair(PUNCT_CLOSE+WS_OPEN, "\",\n\"ws\": \""));
    json_map.push_back(std::make_pair(PUNCT_CLOSE+WORD_OPEN, "\",\n\"w\": \""));
    json_map.push_back(std::make_pair(PUNCT_CLOSE+PUNCT_OPEN, "\",\n\"c\": \""));
    // open json items
    json_map.push_back(std::make_pair(SNT_OPEN, "\"s\": [\n"));
    json_map.push_back(std::make_pair(WS_OPEN, "\"ws\": \""));
    json_map.push_back(std::make_pair(WORD_OPEN, "\"w\": \""));
    json_map.push_back(std::make_pair(PUNCT_OPEN, "\"c\": \""));
    // close json items
    json_map.push_back(std::make_pair(SNT_CLOSE, "]\n"));
    json_map.push_back(std::make_pair(WS_CLOSE, "\"\n"));
    json_map.push_back(std::make_pair(WORD_CLOSE, "\"\n"));
    json_map.push_back(std::make_pair(PUNCT_CLOSE, "\"\n"));
    return json_map;
}

// tsv:
Converter::ConversionMap Converter::createTsvMap() {
    ConversionMap tsv_map;
    /* tsv_map["n"] = "X"; */
    return tsv_map;
}

Converter::ConversionMap Converter::xml_map = Converter::createXmlMap();
Converter::ConversionMap Converter::json_map = Converter::createJsonMap();
Converter::ConversionMap Converter::tsv_map = Converter::createTsvMap();

