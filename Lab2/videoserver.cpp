#include "videoserver.h"
#include <gst/gst.h>

#define LISTEN_PORT 5000

#define VIDEO_WIDTH 640
#define VIDEO_HEIGHT 480
#define VIDEO_FRAMERATE 30

VideoServer::VideoServer(QObject *parent) :
    QObject(parent)
{  
    GstElement *source, *sink, *capsfilter, *tee, *q1, *q2, *encoder, *rtpPayloader, *overlay;
    GstCaps *caps;
    gst_init (NULL, NULL);
    pipeline = gst_pipeline_new ("Ma1n Spr4ying H0se");

    // Create the elements
    source = gst_element_factory_make ("autovideosrc", NULL);
    encoder = gst_element_factory_make ("x264enc", NULL);
    overlay = gst_element_factory_make("textoverlay", NULL);
    sink = gst_element_factory_make ("autovideosink", NULL);
    outboundSink = gst_element_factory_make ("multiudpsink", NULL);
    capsfilter = gst_element_factory_make ("capsfilter", NULL);
    tee = gst_element_factory_make("tee", NULL);
    q1 = gst_element_factory_make("queue", NULL);
    q2 = gst_element_factory_make("queue", NULL);
    rtpPayloader = gst_element_factory_make("rtph264pay", NULL);

    // Setup caps
    caps = gst_caps_new_simple ("video/x-raw-yuv",
            "width", G_TYPE_INT, VIDEO_WIDTH,
            "height", G_TYPE_INT, VIDEO_HEIGHT,
            "framerate", GST_TYPE_FRACTION, VIDEO_FRAMERATE, 1,
            NULL);

    // Set some attributes
    g_object_set(G_OBJECT(overlay), "font-desc","Sans 24", "text","Kamura 1", "valign","top", "halign","left", "shaded-background", true, NULL);
    g_object_set(G_OBJECT(capsfilter), "caps", caps, NULL);
    g_object_set(G_OBJECT(encoder), "speed-preset", 1, "pass", 0, "quantizer", 20, "tune", "zerolatency", NULL);
    g_object_set(G_OBJECT(outboundSink), "clients", "130.240.53.166:6000,130.240.93.175:6001", NULL);

    // Add everything to the pipeline
    gst_bin_add_many (GST_BIN (pipeline), source, encoder, capsfilter, tee, overlay, sink, outboundSink, q1, q2, rtpPayloader, NULL);

    // Request 2 tee sink pads
    GstPad *pad1, *pad2;
    gchar *pad1name, *pad2name;
    pad1 = gst_element_get_request_pad (tee, "src%d");
    pad1name = gst_pad_get_name (pad1);
    pad2 = gst_element_get_request_pad (tee, "src%d");
    pad2name = gst_pad_get_name (pad2);


    // ------------- The mighty pipeline ------------- //
    //
    // Source --> Capsfilter --> Overlay --> Tee -->  Queue1 --> Sink
    //                                          \
    //                                           \--> Queue2 --> Encoder --> RTP-Payloader --> OutboundSink

    // Source -> Capsfilter
    gst_element_link_pads(source, "src", capsfilter, "sink");

    // Capsfilter --> Overlay
    gst_element_link_pads(capsfilter, "src", overlay, "video_sink");

    // Overlay --> Tee
    gst_element_link_pads(overlay, "src", tee, "sink");

    // Tee --> Queue1
    gst_element_link_pads (tee, pad1name, q1, "sink");

    // Tee --> Queue2
    gst_element_link_pads (tee, pad2name, q2, "sink");

    // Queue1 --> Sink
    gst_element_link_pads(q1, "src", sink, "sink");

    // Queue2 --> Encoder
    gst_element_link_pads(q2, "src", encoder, "sink");

    // Encoder --> RTP-Payloader
    gst_element_link_pads(encoder, "src", rtpPayloader, "sink");

    // RTP-Payloader --> OutboundSink
    gst_element_link_pads(rtpPayloader, "src", outboundSink, "sink");

    gst_element_set_state (pipeline, GST_STATE_PLAYING);
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_NON_DEFAULT_PARAMS, "pipeline");
}
