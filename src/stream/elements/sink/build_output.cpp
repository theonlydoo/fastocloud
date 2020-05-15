/*  Copyright (C) 2014-2020 FastoGT. All right reserved.
    This file is part of fastocloud.
    fastocloud is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    fastocloud is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with fastocloud.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stream/elements/sink/build_output.h"

#include <string>

#include "base/output_uri.h"  // for OutputUri, IsFakeUrl

#include "stream/elements/sink/http.h"  // for build_http_sink, HlsOutput
#include "stream/elements/sink/rtmp.h"  // for build_rtmp_sink
#include "stream/elements/sink/srt.h"
#include "stream/elements/sink/tcp.h"
#include "stream/elements/sink/udp.h"  // for build_udp_sink

namespace fastocloud {
namespace stream {
namespace elements {
namespace sink {

Element* build_output(const OutputUri& output, element_id_t sink_id, bool is_vod) {
  common::uri::GURL uri = output.GetOutput();

  if (uri.SchemeIsUdp()) {
    common::net::HostAndPort host(uri.host(), uri.IntPort());
    ElementUDPSink* udp_sink = elements::sink::make_udp_sink(host, sink_id);
    return udp_sink;
  } else if (uri.SchemeIs("tcp")) {
    common::net::HostAndPort host(uri.host(), uri.IntPort());
    ElementTCPServerSink* tcp_sink = elements::sink::make_tcp_server_sink(host, sink_id);
    return tcp_sink;
  } else if (uri.SchemeIs("rtmp")) {
    ElementRtmpSink* rtmp_sink = elements::sink::make_rtmp_sink(sink_id, uri.spec());
    return rtmp_sink;
  } else if (uri.SchemeIsHTTPOrHTTPS()) {
    const auto http_root = output.GetHttpRoot();
    if (!http_root) {
      NOTREACHED() << "Invalid http_root";
      return nullptr;
    }
    const std::string filename = uri.ExtractFileName();
    if (filename.empty()) {
      NOTREACHED() << "Empty playlist name, please create urls like http://localhost/master.m3u8";
      return nullptr;
    }
    elements::sink::HlsOutput hout =
        is_vod ? MakeVodHlsOutput(uri, *http_root, filename) : MakeHlsOutput(uri, *http_root, filename);
    ElementHLSSink* http_sink = elements::sink::make_http_sink(sink_id, hout);
    return http_sink;
  } else if (uri.SchemeIs("srt")) {
    ElementSrtSink* srt_sink = elements::sink::make_srt_sink(uri.spec(), sink_id);
    return srt_sink;
  }

  NOTREACHED() << "Unknown output url: " << uri.spec();
  return nullptr;
}

}  // namespace sink
}  // namespace elements
}  // namespace stream
}  // namespace fastocloud
