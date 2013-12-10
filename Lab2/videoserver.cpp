#include "videoserver.h"
#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>

#define LISTEN_PORT 5000
#define CLIENT_RECEIVE_PORT 6002

#define VIDEO_WIDTH 640
#define VIDEO_HEIGHT 480
#define VIDEO_FRAMERATE 30

VideoServer::VideoServer(QObject *parent) :
    QObject(parent)
{  
    gst_init (NULL, NULL);
}

void VideoServer::init(WId windowID)
{
    GstElement *source, *sink, *capsfilter, *tee, *q1, *q2, *encoder, *rtpPayloader;
    GstCaps *caps;

    pipeline = gst_pipeline_new ("Ma1n Spr4ying H0se");

    // Create the elements
    source = gst_element_factory_make ("autovideosrc", NULL);
    encoder = gst_element_factory_make ("x264enc", NULL);
    overlay = gst_element_factory_make("textoverlay", NULL);
    sink = gst_element_factory_make ("xvimagesink", NULL);
    outboundSink = gst_element_factory_make ("multiudpsink", NULL);
    capsfilter = gst_element_factory_make ("capsfilter", NULL);
    tee = gst_element_factory_make("tee", NULL);
    q1 = gst_element_factory_make("queue", NULL);
    q2 = gst_element_factory_make("queue", NULL);
    rtpPayloader = gst_element_factory_make("rtph264pay", NULL);

    //print_pad_capabilities(source, (gchar*)"src");

    // Setup caps
    caps = gst_caps_new_simple ("video/x-raw-yuv",
            "width", G_TYPE_INT, VIDEO_WIDTH,
            "height", G_TYPE_INT, VIDEO_HEIGHT,
            "framerate", GST_TYPE_FRACTION, VIDEO_FRAMERATE, 1,
            NULL);

    // Set some attributes
    g_object_set(G_OBJECT(overlay), "font-desc","Sans 24", "text","Kamura 1", "valign","top", "halign","left", "shaded-background", true, NULL);
    g_object_set(G_OBJECT(capsfilter), "caps", caps, NULL);
    g_object_set(G_OBJECT(sink), "force-aspect-ratio", true, "sync", false, NULL);
    g_object_set(G_OBJECT(encoder), "speed-preset", 1, "pass", 0, "quantizer", 20, "tune", 0x00000004, NULL);
    //g_object_set(G_OBJECT(outboundSink), "clients", "130.240.53.175:6002", NULL);

    // Add everything to the pipeline
    gst_bin_add_many (GST_BIN (pipeline), source, encoder, capsfilter, tee, overlay, sink, q1, q2, outboundSink, rtpPayloader, NULL);

    // Request 2 tee sink pads
    GstPad *pad1, *pad2;
    gchar *pad1name, *pad2name;
    pad1 = gst_element_get_request_pad (tee, "src%d");
    pad1name = gst_pad_get_name (pad1);
    pad2 = gst_element_get_request_pad (tee, "src%d");
    pad2name = gst_pad_get_name (pad2);

    /* ------------- The mighty pipeline ------------- //

     Source --> Capsfilter --> Overlay --> Tee -->  Queue1 --> Sink
                                              \
                                               \--> Queue2 --> Encoder --> RTP-Payloader --> OutboundSink
    */

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

    if(sink && GST_IS_X_OVERLAY(sink)){
           gst_x_overlay_set_window_handle(GST_X_OVERLAY(sink),windowID);
    } else {
        qDebug() << "It's not an xoverlay thing!";
    }

    gst_element_set_state (pipeline, GST_STATE_PLAYING);
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_NON_DEFAULT_PARAMS, "pipeline");
}

void VideoServer::setTextOverlay(QString text){
    const char* str = text.toHtmlEscaped().toUtf8();
    g_object_set(G_OBJECT(overlay), "text", str, NULL);
}

void VideoServer::sendToNewClient(QString ip, int port)
{
    // Send to the new client
    g_signal_emit_by_name(outboundSink, "add", ip.toStdString().c_str(), port, NULL);
    qDebug() << "Now sending to " + ip + " on port " + QString::number(port);
}

void VideoServer::removeClient(QString ip)
{
    //g_signal_emit_by_name(outboundSink, "remove", ip.toStdString().c_str(), QString(port).toStdString().c_str(), NULL);
}

/* Shows the CURRENT capabilities of the requested pad in the given element */
void VideoServer::print_pad_capabilities (GstElement *element, gchar *pad_name) {
  GstPad *pad = NULL;
  GstCaps *caps = NULL;

  /* Retrieve pad */
  pad = gst_element_get_static_pad (element, pad_name);
  if (!pad) {
    g_printerr ("Could not retrieve pad '%s'\n", pad_name);
    return;
  }

  /* Retrieve negotiated caps (or acceptable caps if negotiation is not finished yet) */
  caps = gst_pad_get_negotiated_caps (pad);
  if (!caps)
    caps = gst_pad_get_caps_reffed (pad);

  /* Print and free */
  g_print ("Caps for the %s pad:\n", pad_name);
  print_caps (caps, "      ");
  gst_caps_unref (caps);
  gst_object_unref (pad);
}

void VideoServer::print_caps (const GstCaps * caps, const gchar * pfx) {
  guint i;

  g_return_if_fail (caps != NULL);

  if (gst_caps_is_any (caps)) {
    g_print ("%sANY\n", pfx);
    return;
  }
  if (gst_caps_is_empty (caps)) {
    g_print ("%sEMPTY\n", pfx);
    return;
  }

  for (i = 0; i < gst_caps_get_size (caps); i++) {
    GstStructure *structure = gst_caps_get_structure (caps, i);

    g_print ("%s%s\n", pfx, gst_structure_get_name (structure));
    gst_structure_foreach (structure, print_field, (gpointer) pfx);
  }
}

gboolean VideoServer::print_field (GQuark field, const GValue * value, gpointer pfx) {
  gchar *str = gst_value_serialize (value);

  g_print ("%s  %15s: %s\n", (gchar *) pfx, g_quark_to_string (field), str);
  g_free (str);
  return TRUE;
}
