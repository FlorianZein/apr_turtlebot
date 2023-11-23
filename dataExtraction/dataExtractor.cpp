#include <iostream>
#include <jsoncpp/json/json.h>
#include <vector>
#include <string>
#include <sstream>
// #include <ros/ros.h>
// #include <sensor_msgs/LaserScan.h>

struct Scan_data {


    float angle_min;
    float angle_max;
    float angle_increment;

    float time_increment;
    float scan_time;
    
    float range_min;
    float range_max;

    std::vector<float> ranges;
    std::vector<float> intensities;
};

int main() {
    // The provided JSON string
    std::string json_str = "---START---{\"header\": {\"seq\": 19420, \"stamp\": {\"secs\": 1677511007, \"nsecs\": 782577255}, \"frame_id\": \"base_scan\"}, \"angle_min\": 0.0, \"angle_max\": 6.2657318115234375, \"angle_increment\": 0.01745329238474369, \"time_increment\": 0.0005592841189354658, \"scan_time\": 0.20134228467941284, \"range_min\": 0.11999999731779099, \"range_max\": 3.5, \"ranges\": [2.2780001163482666, 2.2890000343322754, 2.302999973297119, 2.319999933242798, 2.3289999961853027, 2.3320000171661377, 2.3369998931884766, 2.3580000400543213, 2.380000114440918, 2.4019999504089355, 2.3929998874664307, 2.4110000133514404, 2.443000078201294, 2.4560000896453857, 2.4809999465942383, 2.4719998836517334, 2.51200008392334, 2.5309998989105225, 2.566999912261963, 2.617000102996826, 2.615999937057495, 2.628000020980835, 2.684999942779541, 2.6649999618530273, 2.7230000495910645, 2.765000104904175, 2.7850000858306885, 2.8359999656677246, 2.8469998836517334, 0.16500000655651093, 0.16500000655651093, 0.16200000047683716, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.21400000154972076, 0.21400000154972076, 0.23000000417232513, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.628000020980835, 2.563999891281128, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.9709999561309814, 1.934000015258789, 1.8910000324249268, 1.8869999647140503, 1.8539999723434448, 1.2309999465942383, 1.8040000200271606, 1.909999966621399, 1.8680000305175781, 1.9140000343322754, 1.968999981880188, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.09399999678134918, 0.09399999678134918, 0.09600000083446503, 0.10000000149011612, 0.10400000214576721, 0.10700000077486038, 0.11100000143051147, 0.11400000005960464, 0.11900000274181366, 0.12399999797344208, 0.1289999932050705, 0.1340000033378601, 0.14000000059604645, 0.1459999978542328, 0.15399999916553497, 0.16200000047683716, 0.17100000381469727, 0.18199999630451202, 0.19200000166893005, 0.20399999618530273, 0.2199999988079071, 0.23499999940395355, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 2.121000051498413, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.2160000056028366, 0.2160000056028366, 0.0, 0.0, 0.0, 0.0, 0.0, 2.7730000019073486, 2.7269999980926514, 2.6500000953674316, 2.6489999294281006, 2.568000078201294, 2.552000045776367, 2.5160000324249268, 2.496000051498413, 2.427999973297119, 2.4110000133514404, 2.385999917984009, 2.3510000705718994, 2.3440001010894775, 2.2809998989105225, 2.2829999923706055, 2.234999895095825, 2.24399995803833, 2.255000114440918, 2.190999984741211, 2.187000036239624, 2.1740000247955322, 2.1489999294281006, 2.131999969482422, 2.134000062942505, 2.0969998836517334, 2.0989999771118164, 2.11299991607666, 2.078000068664551, 2.0880000591278076, 2.062999963760376, 2.052000045776367, 2.059999942779541, 2.0409998893737793, 2.0269999504089355, 2.0420000553131104, 2.0390000343322754, 2.0230000019073486, 2.015000104904175, 2.0380001068115234, 2.000999927520752, 1.9279999732971191, 1.8960000276565552, 1.8880000114440918, 1.8969999551773071, 2.2300000190734863, 2.252000093460083, 2.236999988555908, 2.242000102996826, 2.253999948501587, 2.253999948501587, 2.25, 2.2639999389648438, 2.2679998874664307], \"intensities\": [844.0, 837.0, 819.0, 841.0, 824.0, 834.0, 800.0, 797.0, 812.0, 805.0, 776.0, 760.0, 775.0, 758.0, 752.0, 729.0, 715.0, 712.0, 708.0, 704.0, 647.0, 656.0, 649.0, 616.0, 622.0, 602.0, 584.0, 557.0, 191.0, 333.0, 503.0, 1962.0, 78.0, 0.0, 0.0, 65.0, 42.0, 87.0, 47.0, 0.0, 38.0, 40.0, 0.0, 45.0, 69.0, 77.0, 59.0, 49.0, 64.0, 84.0, 53.0, 0.0, 0.0, 0.0, 34.0, 0.0, 47.0, 0.0, 0.0, 0.0, 0.0, 47.0, 0.0, 0.0, 47.0, 0.0, 0.0, 0.0, 0.0, 2033.0, 10760.0, 1179.0, 0.0, 0.0, 0.0, 0.0, 0.0, 47.0, 0.0, 0.0, 0.0, 787.0, 474.0, 162.0, 339.0, 327.0, 351.0, 338.0, 353.0, 340.0, 328.0, 459.0, 597.0, 747.0, 727.0, 154.0, 852.0, 159.0, 565.0, 676.0, 545.0, 435.0, 84.0, 47.0, 47.0, 0.0, 64.0, 88.0, 0.0, 0.0, 55.0, 47.0, 128.0, 747.0, 249.0, 0.0, 60.0, 0.0, 0.0, 47.0, 47.0, 0.0, 0.0, 47.0, 0.0, 47.0, 0.0, 0.0, 47.0, 0.0, 47.0, 0.0, 0.0, 47.0, 0.0, 0.0, 0.0, 47.0, 47.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 47.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 47.0, 0.0, 0.0, 47.0, 0.0, 0.0, 0.0, 0.0, 47.0, 47.0, 0.0, 0.0, 0.0, 47.0, 0.0, 0.0, 0.0, 47.0, 47.0, 0.0, 0.0, 47.0, 0.0, 47.0, 0.0, 0.0, 47.0, 0.0, 47.0, 0.0, 47.0, 0.0, 0.0, 0.0, 0.0, 47.0, 47.0, 47.0, 0.0, 0.0, 0.0, 0.0, 0.0, 47.0, 47.0, 0.0, 47.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 47.0, 0.0, 0.0, 0.0, 47.0, 0.0, 41.0, 57.0, 83.0, 0.0, 316.0, 758.0, 1696.0, 279.0, 1531.0, 2154.0, 2278.0, 2443.0, 2732.0, 2681.0, 2618.0, 2549.0, 2335.0, 2079.0, 1959.0, 1638.0, 1472.0, 1372.0, 1252.0, 1088.0, 978.0, 759.0, 584.0, 515.0, 242.0, 224.0, 245.0, 101.0, 101.0, 79.0, 56.0, 0.0, 0.0, 0.0, 66.0, 88.0, 86.0, 79.0, 101.0, 111.0, 47.0, 285.0, 0.0, 67.0, 152.0, 47.0, 0.0, 0.0, 47.0, 0.0, 47.0, 0.0, 47.0, 47.0, 0.0, 0.0, 0.0, 0.0, 47.0, 0.0, 47.0, 0.0, 47.0, 47.0, 0.0, 0.0, 47.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 47.0, 0.0, 0.0, 49.0, 0.0, 0.0, 37.0, 57.0, 672.0, 105.0, 92.0, 47.0, 0.0, 0.0, 0.0, 270.0, 433.0, 467.0, 476.0, 479.0, 506.0, 527.0, 502.0, 522.0, 528.0, 539.0, 558.0, 559.0, 565.0, 570.0, 561.0, 583.0, 609.0, 653.0, 668.0, 676.0, 700.0, 698.0, 728.0, 724.0, 743.0, 752.0, 759.0, 764.0, 772.0, 785.0, 775.0, 791.0, 810.0, 805.0, 827.0, 826.0, 828.0, 832.0, 842.0, 2417.0, 2054.0, 2062.0, 2682.0, 839.0, 899.0, 846.0, 852.0, 865.0, 860.0, 877.0, 882.0, 833.0]}___END___";

    // Parse JSON string
    Json::CharReaderBuilder builder;
    Json::CharReader *reader = builder.newCharReader();
    Json::Value root;
    std::string jsonError;
    std::istringstream json_stream(json_str);
    // const char* json_stream(json_str);
    const char* jsonCharBegin = &json_str.front();
    const char* jsonCharEnd = jsonCharBegin + json_str.length();
    // std::cout << "test" << std::endl;
    bool boolt = reader->parse(jsonCharBegin, jsonCharEnd, &root, &jsonError);
    std::cout << boolt << std::endl;
    // std::cout << "test" << std::endl;
    std::cout << jsonError << std::endl;
    delete reader;
    std::cout << "test" << std::endl;

    if(boolt)
    {
        std::cout << root.size() << std::endl;
    }



    Json::Value::ArrayIndex i = 1;
    std::cout << root["angle_min"].asFloat() << std::endl;

    // Create LaserScan message
    Scan_data scan_data;

    // Populate header
    // scan_data.header.seq = root["header"]["seq"].asUInt();
    // scan_data.header.stamp.sec = root["header"]["stamp"]["secs"].asUInt();
    // scan_data.header.stamp.nsec = root["header"]["stamp"]["nsecs"].asUInt();
    // scan_data.header.frame_id = root["header"]["frame_id"].asString();

    // Populate other fields
    std::cout << "test" << std::endl;
    scan_data.angle_min = root["angle_min"].asFloat();
    std::cout << "test" << std::endl;
    scan_data.angle_max = root["angle_max"].asFloat();
    scan_data.angle_increment = root["angle_increment"].asFloat();
    scan_data.time_increment = root["time_increment"].asFloat();
    scan_data.scan_time = root["scan_time"].asFloat();
    scan_data.range_min = root["range_min"].asFloat();
    scan_data.range_max = root["range_max"].asFloat();

    // Populate ranges and intensities
    for (const auto &range : root["ranges"]) {
        scan_data.ranges.push_back(range.asFloat());
    }
    for (const auto &intensity : root["intensities"]) {
        scan_data.intensities.push_back(intensity.asFloat());
    }

    // Display LaserScan message content
    std::cout << "LaserScan Message Content:" << std::endl;
    std::cout << "min angle Seq: " << scan_data.angle_min << std::endl;
    std::cout << "range [1] " << scan_data.ranges[1] << std::endl;

    // Add more fields as needed

    return 0;
}