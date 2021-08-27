#include "torrent/torrent.hpp"

int main()
{
    bittorrent::Metainfo torrent;
    char filename[] = "test/files/5.Centimeters.Per.Second.2007.1080p.Bluray.AVC.LPCM.4.0-RY4N.torrent";

    torrent.readFile(filename);
    return 0;
}