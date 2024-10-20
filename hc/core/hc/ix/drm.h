// drm.h
#define DRM_IOCTL_BASE 'd'
#define DRM_IO(nr) _IO(DRM_IOCTL_BASE,nr)
#define DRM_IOR(nr,type) _IOR(DRM_IOCTL_BASE,nr,type)
#define DRM_IOW(nr,type) _IOW(DRM_IOCTL_BASE,nr,type)
#define DRM_IOWR(nr,type) _IOWR(DRM_IOCTL_BASE,nr,type)

#define DRM_IOCTL_VERSION DRM_IOWR(0x00, struct drm_version)
#define DRM_IOCTL_GET_UNIQUE DRM_IOWR(0x01, struct drm_unique)
#define DRM_IOCTL_GET_MAGIC DRM_IOR(0x02, struct drm_auth)
#define DRM_IOCTL_IRQ_BUSID DRM_IOWR(0x03, struct drm_irq_busid)
#define DRM_IOCTL_GET_MAP DRM_IOWR(0x04, struct drm_map)
#define DRM_IOCTL_GET_CLIENT DRM_IOWR(0x05, struct drm_client)
#define DRM_IOCTL_GET_STATS DRM_IOR(0x06, struct drm_stats)
#define DRM_IOCTL_SET_VERSION DRM_IOWR(0x07, struct drm_set_version)
#define DRM_IOCTL_MODESET_CTL DRM_IOW(0x08, struct drm_modeset_ctl)
#define DRM_IOCTL_GEM_CLOSE DRM_IOW (0x09, struct drm_gem_close)
#define DRM_IOCTL_GEM_FLINK DRM_IOWR(0x0a, struct drm_gem_flink)
#define DRM_IOCTL_GEM_OPEN DRM_IOWR(0x0b, struct drm_gem_open)
#define DRM_IOCTL_GET_CAP DRM_IOWR(0x0c, struct drm_get_cap)
#define DRM_IOCTL_SET_CLIENT_CAP DRM_IOW(0x0d, struct drm_set_client_cap)

#define DRM_IOCTL_SET_UNIQUE DRM_IOW(0x10, struct drm_unique)
#define DRM_IOCTL_AUTH_MAGIC DRM_IOW(0x11, struct drm_auth)
#define DRM_IOCTL_BLOCK DRM_IOWR(0x12, struct drm_block)
#define DRM_IOCTL_UNBLOCK DRM_IOWR(0x13, struct drm_block)
#define DRM_IOCTL_CONTROL DRM_IOW(0x14, struct drm_control)
#define DRM_IOCTL_ADD_MAP DRM_IOWR(0x15, struct drm_map)
#define DRM_IOCTL_ADD_BUFS DRM_IOWR(0x16, struct drm_buf_desc)
#define DRM_IOCTL_MARK_BUFS DRM_IOW(0x17, struct drm_buf_desc)
#define DRM_IOCTL_INFO_BUFS DRM_IOWR(0x18, struct drm_buf_info)
#define DRM_IOCTL_MAP_BUFS DRM_IOWR(0x19, struct drm_buf_map)
#define DRM_IOCTL_FREE_BUFS DRM_IOW(0x1a, struct drm_buf_free)

#define DRM_IOCTL_RM_MAP DRM_IOW(0x1b, struct drm_map)

#define DRM_IOCTL_SET_SAREA_CTX DRM_IOW(0x1c, struct drm_ctx_priv_map)
#define DRM_IOCTL_GET_SAREA_CTX DRM_IOWR(0x1d, struct drm_ctx_priv_map)

#define DRM_IOCTL_SET_MASTER DRM_IO(0x1e)
#define DRM_IOCTL_DROP_MASTER DRM_IO(0x1f)

#define DRM_IOCTL_ADD_CTX DRM_IOWR(0x20, struct drm_ctx)
#define DRM_IOCTL_RM_CTX DRM_IOWR(0x21, struct drm_ctx)
#define DRM_IOCTL_MOD_CTX DRM_IOW(0x22, struct drm_ctx)
#define DRM_IOCTL_GET_CTX DRM_IOWR(0x23, struct drm_ctx)
#define DRM_IOCTL_SWITCH_CTX DRM_IOW(0x24, struct drm_ctx)
#define DRM_IOCTL_NEW_CTX DRM_IOW(0x25, struct drm_ctx)
#define DRM_IOCTL_RES_CTX DRM_IOWR(0x26, struct drm_ctx_res)
#define DRM_IOCTL_ADD_DRAW DRM_IOWR(0x27, struct drm_draw)
#define DRM_IOCTL_RM_DRAW DRM_IOWR(0x28, struct drm_draw)
#define DRM_IOCTL_DMA DRM_IOWR(0x29, struct drm_dma)
#define DRM_IOCTL_LOCK DRM_IOW(0x2a, struct drm_lock)
#define DRM_IOCTL_UNLOCK DRM_IOW(0x2b, struct drm_lock)
#define DRM_IOCTL_FINISH DRM_IOW(0x2c, struct drm_lock)

#define DRM_IOCTL_PRIME_HANDLE_TO_FD DRM_IOWR(0x2d, struct drm_prime_handle)
#define DRM_IOCTL_PRIME_FD_TO_HANDLE DRM_IOWR(0x2e, struct drm_prime_handle)

#define DRM_IOCTL_AGP_ACQUIRE DRM_IO(0x30)
#define DRM_IOCTL_AGP_RELEASE DRM_IO(0x31)
#define DRM_IOCTL_AGP_ENABLE DRM_IOW(0x32, struct drm_agp_mode)
#define DRM_IOCTL_AGP_INFO DRM_IOR(0x33, struct drm_agp_info)
#define DRM_IOCTL_AGP_ALLOC DRM_IOWR(0x34, struct drm_agp_buffer)
#define DRM_IOCTL_AGP_FREE DRM_IOW(0x35, struct drm_agp_buffer)
#define DRM_IOCTL_AGP_BIND DRM_IOW(0x36, struct drm_agp_binding)
#define DRM_IOCTL_AGP_UNBIND DRM_IOW(0x37, struct drm_agp_binding)

#define DRM_IOCTL_SG_ALLOC DRM_IOWR(0x38, struct drm_scatter_gather)
#define DRM_IOCTL_SG_FREE DRM_IOW(0x39, struct drm_scatter_gather)

#define DRM_IOCTL_WAIT_VBLANK DRM_IOWR(0x3a, union drm_wait_vblank)

#define DRM_IOCTL_CRTC_GET_SEQUENCE DRM_IOWR(0x3b, struct drm_crtc_get_sequence)
#define DRM_IOCTL_CRTC_QUEUE_SEQUENCE DRM_IOWR(0x3c, struct drm_crtc_queue_sequence)

#define DRM_IOCTL_UPDATE_DRAW DRM_IOW(0x3f, struct drm_update_draw)

#define DRM_IOCTL_MODE_GETRESOURCES DRM_IOWR(0xA0, struct drm_mode_card_res)
#define DRM_IOCTL_MODE_GETCRTC DRM_IOWR(0xA1, struct drm_mode_crtc)
#define DRM_IOCTL_MODE_SETCRTC DRM_IOWR(0xA2, struct drm_mode_crtc)
#define DRM_IOCTL_MODE_CURSOR DRM_IOWR(0xA3, struct drm_mode_cursor)
#define DRM_IOCTL_MODE_GETGAMMA DRM_IOWR(0xA4, struct drm_mode_crtc_lut)
#define DRM_IOCTL_MODE_SETGAMMA DRM_IOWR(0xA5, struct drm_mode_crtc_lut)
#define DRM_IOCTL_MODE_GETENCODER DRM_IOWR(0xA6, struct drm_mode_get_encoder)
#define DRM_IOCTL_MODE_GETCONNECTOR DRM_IOWR(0xA7, struct drm_mode_get_connector)

#define DRM_IOCTL_MODE_GETPROPERTY DRM_IOWR(0xAA, struct drm_mode_get_property)
#define DRM_IOCTL_MODE_SETPROPERTY DRM_IOWR(0xAB, struct drm_mode_connector_set_property)
#define DRM_IOCTL_MODE_GETPROPBLOB DRM_IOWR(0xAC, struct drm_mode_get_blob)
#define DRM_IOCTL_MODE_GETFB DRM_IOWR(0xAD, struct drm_mode_fb_cmd)
#define DRM_IOCTL_MODE_ADDFB DRM_IOWR(0xAE, struct drm_mode_fb_cmd)
#define DRM_IOCTL_MODE_RMFB DRM_IOWR(0xAF, unsigned int)
#define DRM_IOCTL_MODE_PAGE_FLIP DRM_IOWR(0xB0, struct drm_mode_crtc_page_flip)
#define DRM_IOCTL_MODE_DIRTYFB DRM_IOWR(0xB1, struct drm_mode_fb_dirty_cmd)

#define DRM_IOCTL_MODE_CREATE_DUMB DRM_IOWR(0xB2, struct drm_mode_create_dumb)
#define DRM_IOCTL_MODE_MAP_DUMB DRM_IOWR(0xB3, struct drm_mode_map_dumb)
#define DRM_IOCTL_MODE_DESTROY_DUMB DRM_IOWR(0xB4, struct drm_mode_destroy_dumb)
#define DRM_IOCTL_MODE_GETPLANERESOURCES DRM_IOWR(0xB5, struct drm_mode_get_plane_res)
#define DRM_IOCTL_MODE_GETPLANE DRM_IOWR(0xB6, struct drm_mode_get_plane)
#define DRM_IOCTL_MODE_SETPLANE DRM_IOWR(0xB7, struct drm_mode_set_plane)
#define DRM_IOCTL_MODE_ADDFB2 DRM_IOWR(0xB8, struct drm_mode_fb_cmd2)
#define DRM_IOCTL_MODE_OBJ_GETPROPERTIES DRM_IOWR(0xB9, struct drm_mode_obj_get_properties)
#define DRM_IOCTL_MODE_OBJ_SETPROPERTY DRM_IOWR(0xBA, struct drm_mode_obj_set_property)
#define DRM_IOCTL_MODE_CURSOR2 DRM_IOWR(0xBB, struct drm_mode_cursor2)
#define DRM_IOCTL_MODE_ATOMIC DRM_IOWR(0xBC, struct drm_mode_atomic)
#define DRM_IOCTL_MODE_CREATEPROPBLOB DRM_IOWR(0xBD, struct drm_mode_create_blob)
#define DRM_IOCTL_MODE_DESTROYPROPBLOB DRM_IOWR(0xBE, struct drm_mode_destroy_blob)

#define DRM_IOCTL_SYNCOBJ_CREATE DRM_IOWR(0xBF, struct drm_syncobj_create)
#define DRM_IOCTL_SYNCOBJ_DESTROY DRM_IOWR(0xC0, struct drm_syncobj_destroy)
#define DRM_IOCTL_SYNCOBJ_HANDLE_TO_FD DRM_IOWR(0xC1, struct drm_syncobj_handle)
#define DRM_IOCTL_SYNCOBJ_FD_TO_HANDLE DRM_IOWR(0xC2, struct drm_syncobj_handle)
#define DRM_IOCTL_SYNCOBJ_WAIT DRM_IOWR(0xC3, struct drm_syncobj_wait)
#define DRM_IOCTL_SYNCOBJ_RESET DRM_IOWR(0xC4, struct drm_syncobj_array)
#define DRM_IOCTL_SYNCOBJ_SIGNAL DRM_IOWR(0xC5, struct drm_syncobj_array)

#define DRM_IOCTL_MODE_CREATE_LEASE DRM_IOWR(0xC6, struct drm_mode_create_lease)
#define DRM_IOCTL_MODE_LIST_LESSEES DRM_IOWR(0xC7, struct drm_mode_list_lessees)
#define DRM_IOCTL_MODE_GET_LEASE DRM_IOWR(0xC8, struct drm_mode_get_lease)
#define DRM_IOCTL_MODE_REVOKE_LEASE DRM_IOWR(0xC9, struct drm_mode_revoke_lease)

#define DRM_IOCTL_SYNCOBJ_TIMELINE_WAIT DRM_IOWR(0xCA, struct drm_syncobj_timeline_wait)
#define DRM_IOCTL_SYNCOBJ_QUERY DRM_IOWR(0xCB, struct drm_syncobj_timeline_array)
#define DRM_IOCTL_SYNCOBJ_TRANSFER DRM_IOWR(0xCC, struct drm_syncobj_transfer)
#define DRM_IOCTL_SYNCOBJ_TIMELINE_SIGNAL DRM_IOWR(0xCD, struct drm_syncobj_timeline_array)

#define DRM_IOCTL_MODE_GETFB2 DRM_IOWR(0xCE, struct drm_mode_fb_cmd2)

enum drm_vblank_seq_type {
    DRM_VBLANK_ABSOLUTE = 0x0, /**< Wait for specific vblank sequence number */
    DRM_VBLANK_RELATIVE = 0x1, /**< Wait for given number of vblanks */
    /* bits 1-6 are reserved for high crtcs */
    DRM_VBLANK_HIGH_CRTC_MASK = 0x0000003e,
    DRM_VBLANK_EVENT = 0x4000000, /**< Send event instead of blocking */
    DRM_VBLANK_FLIP = 0x8000000, /**< Scheduled buffer swap should flip */
    DRM_VBLANK_NEXTONMISS = 0x10000000, /**< If missed, wait for next vblank */
    DRM_VBLANK_SECONDARY = 0x20000000, /**< Secondary display controller */
    DRM_VBLANK_SIGNAL = 0x40000000 /**< Send signal instead of blocking, unsupported */
};
#define DRM_VBLANK_HIGH_CRTC_SHIFT 1

#define DRM_VBLANK_TYPES_MASK (DRM_VBLANK_ABSOLUTE | DRM_VBLANK_RELATIVE)
#define DRM_VBLANK_FLAGS_MASK (DRM_VBLANK_EVENT | DRM_VBLANK_SIGNAL | DRM_VBLANK_SECONDARY | DRM_VBLANK_NEXTONMISS)

struct drm_wait_vblank_request {
    enum drm_vblank_seq_type type;
    uint32_t sequence;
    uint64_t signal;
};

struct drm_wait_vblank_reply {
    enum drm_vblank_seq_type type;
    uint32_t sequence;
    int64_t tval_sec;
    int64_t tval_usec;
};

// DRM_IOCTL_WAIT_VBLANK ioctl argument type.
union drm_wait_vblank {
    struct drm_wait_vblank_request request;
    struct drm_wait_vblank_reply reply;
};

struct drm_event {
    uint32_t type;
    uint32_t length; // Including header.
};

#define DRM_EVENT_VBLANK 0x01
#define DRM_EVENT_FLIP_COMPLETE 0x02
#define DRM_EVENT_CRTC_SEQUENCE 0x03

struct drm_event_vblank {
    struct drm_event base;
    uint64_t user_data;
    uint32_t tv_sec;
    uint32_t tv_usec;
    uint32_t sequence;
    uint32_t crtc_id; /* 0 on older kernels that do not support this */
};

// drm_mode.h
#define DRM_CONNECTOR_NAME_LEN 32
#define DRM_DISPLAY_MODE_LEN 32
#define DRM_PROP_NAME_LEN 32

#define DRM_MODE_TYPE_PREFERRED (1<<3)
#define DRM_MODE_TYPE_USERDEF (1<<5)
#define DRM_MODE_TYPE_DRIVER (1<<6)

#define DRM_MODE_TYPE_ALL (DRM_MODE_TYPE_PREFERRED | DRM_MODE_TYPE_USERDEF | DRM_MODE_TYPE_DRIVER)

// Video mode flags
#define DRM_MODE_FLAG_PHSYNC (1<<0)
#define DRM_MODE_FLAG_NHSYNC (1<<1)
#define DRM_MODE_FLAG_PVSYNC (1<<2)
#define DRM_MODE_FLAG_NVSYNC (1<<3)
#define DRM_MODE_FLAG_INTERLACE (1<<4)
#define DRM_MODE_FLAG_DBLSCAN (1<<5)
#define DRM_MODE_FLAG_CSYNC (1<<6)
#define DRM_MODE_FLAG_PCSYNC (1<<7)
#define DRM_MODE_FLAG_NCSYNC (1<<8)
#define DRM_MODE_FLAG_HSKEW (1<<9) // hskew provided
#define DRM_MODE_FLAG_DBLCLK (1<<12)
#define DRM_MODE_FLAG_CLKDIV2 (1<<13)

#define DRM_MODE_FLAG_3D_MASK (0x1f<<14)
#define DRM_MODE_FLAG_3D_NONE (0<<14)
#define DRM_MODE_FLAG_3D_FRAME_PACKING (1<<14)
#define DRM_MODE_FLAG_3D_FIELD_ALTERNATIVE (2<<14)
#define DRM_MODE_FLAG_3D_LINE_ALTERNATIVE (3<<14)
#define DRM_MODE_FLAG_3D_SIDE_BY_SIDE_FULL (4<<14)
#define DRM_MODE_FLAG_3D_L_DEPTH (5<<14)
#define DRM_MODE_FLAG_3D_L_DEPTH_GFX_GFX_DEPTH (6<<14)
#define DRM_MODE_FLAG_3D_TOP_AND_BOTTOM (7<<14)
#define DRM_MODE_FLAG_3D_SIDE_BY_SIDE_HALF (8<<14)

// Picture aspect ratio options
#define DRM_MODE_PICTURE_ASPECT_NONE 0
#define DRM_MODE_PICTURE_ASPECT_4_3 1
#define DRM_MODE_PICTURE_ASPECT_16_9 2
#define DRM_MODE_PICTURE_ASPECT_64_27 3
#define DRM_MODE_PICTURE_ASPECT_256_135 4

// Content type options
#define DRM_MODE_CONTENT_TYPE_NO_DATA 0
#define DRM_MODE_CONTENT_TYPE_GRAPHICS 1
#define DRM_MODE_CONTENT_TYPE_PHOTO 2
#define DRM_MODE_CONTENT_TYPE_CINEMA 3
#define DRM_MODE_CONTENT_TYPE_GAME 4

// Aspect ratio flag bitmask (4 bits 22:19)
#define DRM_MODE_FLAG_PIC_AR_MASK (0x0F<<19)
#define DRM_MODE_FLAG_PIC_AR_NONE (DRM_MODE_PICTURE_ASPECT_NONE<<19)
#define DRM_MODE_FLAG_PIC_AR_4_3 (DRM_MODE_PICTURE_ASPECT_4_3<<19)
#define DRM_MODE_FLAG_PIC_AR_16_9 (DRM_MODE_PICTURE_ASPECT_16_9<<19)
#define DRM_MODE_FLAG_PIC_AR_64_27 (DRM_MODE_PICTURE_ASPECT_64_27<<19)
#define DRM_MODE_FLAG_PIC_AR_256_135 (DRM_MODE_PICTURE_ASPECT_256_135<<19)

#define DRM_MODE_FLAG_ALL ( \
    DRM_MODE_FLAG_PHSYNC | \
    DRM_MODE_FLAG_NHSYNC | \
    DRM_MODE_FLAG_PVSYNC | \
    DRM_MODE_FLAG_NVSYNC | \
    DRM_MODE_FLAG_INTERLACE | \
    DRM_MODE_FLAG_DBLSCAN | \
    DRM_MODE_FLAG_CSYNC | \
    DRM_MODE_FLAG_PCSYNC | \
    DRM_MODE_FLAG_NCSYNC | \
    DRM_MODE_FLAG_HSKEW | \
    DRM_MODE_FLAG_DBLCLK | \
    DRM_MODE_FLAG_CLKDIV2 | \
    DRM_MODE_FLAG_3D_MASK \
)

// DPMS flags
#define DRM_MODE_DPMS_ON 0
#define DRM_MODE_DPMS_STANDBY 1
#define DRM_MODE_DPMS_SUSPEND 2
#define DRM_MODE_DPMS_OFF 3

// Scaling mode options
#define DRM_MODE_SCALE_NONE 0 // Unmodified timing (display or software can still scale)
#define DRM_MODE_SCALE_FULLSCREEN 1 // Full screen, ignore aspect
#define DRM_MODE_SCALE_CENTER 2 // Centered, no scaling
#define DRM_MODE_SCALE_ASPECT 3 // Full screen, preserve aspect

// Dithering mode options
#define DRM_MODE_DITHERING_OFF 0
#define DRM_MODE_DITHERING_ON 1
#define DRM_MODE_DITHERING_AUTO 2

// Dirty info options
#define DRM_MODE_DIRTY_OFF      0
#define DRM_MODE_DIRTY_ON       1
#define DRM_MODE_DIRTY_ANNOTATE 2

// Link Status options
#define DRM_MODE_LINK_STATUS_GOOD 0
#define DRM_MODE_LINK_STATUS_BAD 1

#define DRM_MODE_ROTATE_0       (1<<0)
#define DRM_MODE_ROTATE_90      (1<<1)
#define DRM_MODE_ROTATE_180     (1<<2)
#define DRM_MODE_ROTATE_270     (1<<3)

#define DRM_MODE_ROTATE_MASK (DRM_MODE_ROTATE_0 | DRM_MODE_ROTATE_90 | DRM_MODE_ROTATE_180 | DRM_MODE_ROTATE_270)

#define DRM_MODE_REFLECT_X      (1<<4)
#define DRM_MODE_REFLECT_Y      (1<<5)

#define DRM_MODE_REFLECT_MASK (DRM_MODE_REFLECT_X | DRM_MODE_REFLECT_Y)

// Content Protection Flags
#define DRM_MODE_CONTENT_PROTECTION_UNDESIRED 0
#define DRM_MODE_CONTENT_PROTECTION_DESIRED     1
#define DRM_MODE_CONTENT_PROTECTION_ENABLED     2

/**
 * struct drm_mode_modeinfo - Display mode information.
 * clock: pixel clock in kHz
 * hdisplay: horizontal display size
 * hsync_start: horizontal sync start
 * hsync_end: horizontal sync end
 * htotal: horizontal total size
 * hskew: horizontal skew
 * vdisplay: vertical display size
 * vsync_start: vertical sync start
 * vsync_end: vertical sync end
 * vtotal: vertical total size
 * vscan: vertical scan
 * vrefresh: approximate vertical refresh rate in Hz
 * flags: bitmask of misc. flags, see DRM_MODE_FLAG_* defines
 * type: bitmask of type flags, see DRM_MODE_TYPE_* defines
 * name: string describing the mode resolution
 *
 * This is the user-space API display mode information structure. For the
 * kernel version see struct drm_display_mode.
 */
struct drm_mode_modeinfo {
    uint32_t clock;
    uint16_t hdisplay;
    uint16_t hsync_start;
    uint16_t hsync_end;
    uint16_t htotal;
    uint16_t hskew;
    uint16_t vdisplay;
    uint16_t vsync_start;
    uint16_t vsync_end;
    uint16_t vtotal;
    uint16_t vscan;

    int32_t vrefresh;

    uint32_t flags;
    uint32_t type;
    char name[DRM_DISPLAY_MODE_LEN];
};

struct drm_mode_card_res {
    uint32_t *fb_id_ptr;
    uint32_t *crtc_id_ptr;
    uint32_t *connector_id_ptr;
    uint32_t *encoder_id_ptr;
    uint32_t count_fbs;
    uint32_t count_crtcs;
    uint32_t count_connectors;
    uint32_t count_encoders;
    uint32_t min_width;
    uint32_t max_width;
    uint32_t min_height;
    uint32_t max_height;
};

struct drm_mode_crtc {
    uint32_t *set_connectors_ptr;
    uint32_t count_connectors;

    uint32_t crtc_id; // Id
    uint32_t fb_id; // Id of framebuffer

    uint32_t x; // x Position on the framebuffer
    uint32_t y; // y Position on the framebuffer

    uint32_t gamma_size;
    uint32_t mode_valid;
    struct drm_mode_modeinfo mode;
};

#define DRM_MODE_CONNECTOR_Unknown 0
#define DRM_MODE_CONNECTOR_VGA 1
#define DRM_MODE_CONNECTOR_DVII 2
#define DRM_MODE_CONNECTOR_DVID 3
#define DRM_MODE_CONNECTOR_DVIA 4
#define DRM_MODE_CONNECTOR_Composite 5
#define DRM_MODE_CONNECTOR_SVIDEO 6
#define DRM_MODE_CONNECTOR_LVDS 7
#define DRM_MODE_CONNECTOR_Component 8
#define DRM_MODE_CONNECTOR_9PinDIN 9
#define DRM_MODE_CONNECTOR_DisplayPort 10
#define DRM_MODE_CONNECTOR_HDMIA 11
#define DRM_MODE_CONNECTOR_HDMIB 12
#define DRM_MODE_CONNECTOR_TV 13
#define DRM_MODE_CONNECTOR_eDP 14
#define DRM_MODE_CONNECTOR_VIRTUAL      15
#define DRM_MODE_CONNECTOR_DSI 16
#define DRM_MODE_CONNECTOR_DPI 17
#define DRM_MODE_CONNECTOR_WRITEBACK 18
#define DRM_MODE_CONNECTOR_SPI 19
#define DRM_MODE_CONNECTOR_USB 20

/**
 * struct drm_mode_get_connector - Get connector metadata.
 *
 * User-space can perform a GETCONNECTOR ioctl to retrieve information about a
 * connector. User-space is expected to retrieve encoders, modes and properties
 * by performing this ioctl at least twice: the first time to retrieve the
 * number of elements, the second time to retrieve the elements themselves.
 *
 * To retrieve the number of elements, set count_props and count_encoders to
 * zero, set count_modes to 1, and set modes_ptr to a temporary struct
 * drm_mode_modeinfo element.
 *
 * To retrieve the elements, allocate arrays for encoders_ptr, modes_ptr,
 * props_ptr and prop_values_ptr, then set count_modes, count_props and
 * count_encoders to their capacity.
 *
 * Performing the ioctl only twice may be racy: the number of elements may have
 * changed with a hotplug event in-between the two ioctls. User-space is
 * expected to retry the last ioctl until the number of elements stabilizes.
 * The kernel won't fill any array which doesn't have the expected length.
 *
 * **Force-probing a connector**
 *
 * If the count_modes field is set to zero and the DRM client is the current
 * DRM master, the kernel will perform a forced probe on the connector to
 * refresh the connector status, modes and EDID. A forced-probe can be slow,
 * might cause flickering and the ioctl will block.
 *
 * User-space needs to force-probe connectors to ensure their metadata is
 * up-to-date at startup and after receiving a hot-plug event. User-space
 * may perform a forced-probe when the user explicitly requests it. User-space
 * shouldn't perform a forced-probe in other situations.
 */
struct drm_mode_get_connector {
    /** encoders_ptr: Pointer to ``uint32_t`` array of object IDs. */
    uint32_t *encoders_ptr;
    /** modes_ptr: Pointer to struct drm_mode_modeinfo array. */
    struct drm_mode_modeinfo *modes_ptr;
    /** props_ptr: Pointer to ``uint32_t`` array of property IDs. */
    uint32_t *props_ptr;
    /** prop_values_ptr: Pointer to ``uint64_t`` array of property values. */
    uint64_t *prop_values_ptr;

    /** count_modes: Number of modes. */
    int32_t count_modes;
    /** count_props: Number of properties. */
    int32_t count_props;
    /** count_encoders: Number of encoders. */
    int32_t count_encoders;

    /** encoder_id: Object ID of the current encoder. */
    uint32_t encoder_id;
    /** connector_id: Object ID of the connector. */
    uint32_t connector_id;
    /**
     * connector_type: Type of the connector.
     *
     * See DRM_MODE_CONNECTOR_* defines.
     */
    uint32_t connector_type;
    /**
     * connector_type_id: Type-specific connector number.
     *
     * This is not an object ID. This is a per-type connector number. Each
     * (type, type_id) combination is unique across all connectors of a DRM
     * device.
     */
    uint32_t connector_type_id;

    /**
     * connection: Status of the connector.
     *
     * See enum drm_connector_status.
     */
    uint32_t connection;
    /** mm_width: Width of the connected sink in millimeters. */
    uint32_t mm_width;
    /** mm_height: Height of the connected sink in millimeters. */
    uint32_t mm_height;
    /**
     * subpixel: Subpixel order of the connected sink.
     *
     * See enum subpixel_order.
     */
    uint32_t subpixel;

    /** pad: Padding, must be zero. */
    uint32_t pad;
};

struct drm_mode_fb_cmd {
    uint32_t fb_id;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t bpp;
    uint32_t depth;

    uint32_t handle; // driver specific handle
};

#define DRM_MODE_FB_DIRTY_ANNOTATE_COPY 0x01
#define DRM_MODE_FB_DIRTY_ANNOTATE_FILL 0x02
#define DRM_MODE_FB_DIRTY_FLAGS         0x03

#define DRM_MODE_FB_DIRTY_MAX_CLIPS     256

/*
 * Mark a region of a framebuffer as dirty.
 *
 * Some hardware does not automatically update display contents
 * as a hardware or software draw to a framebuffer. This ioctl
 * allows userspace to tell the kernel and the hardware what
 * regions of the framebuffer have changed.
 *
 * The kernel or hardware is free to update more then just the
 * region specified by the clip rects. The kernel or hardware
 * may also delay and/or coalesce several calls to dirty into a
 * single update.
 *
 * Userspace may annotate the updates, the annotates are a
 * promise made by the caller that the change is either a copy
 * of pixels or a fill of a single color in the region specified.
 *
 * If the DRM_MODE_FB_DIRTY_ANNOTATE_COPY flag is given then
 * the number of updated regions are half of num_clips given,
 * where the clip rects are paired in src and dst. The width and
 * height of each one of the pairs must match.
 *
 * If the DRM_MODE_FB_DIRTY_ANNOTATE_FILL flag is given the caller
 * promises that the region specified of the clip rects is filled
 * completely with a single color as given in the color argument.
 */

struct drm_mode_fb_dirty_cmd {
    uint32_t fb_id;
    uint32_t flags;
    uint32_t color;
    uint32_t num_clips;
    uint64_t clips_ptr;
};

#define DRM_MODE_PAGE_FLIP_EVENT 0x01
#define DRM_MODE_PAGE_FLIP_ASYNC 0x02
#define DRM_MODE_PAGE_FLIP_TARGET_ABSOLUTE 0x4
#define DRM_MODE_PAGE_FLIP_TARGET_RELATIVE 0x8
#define DRM_MODE_PAGE_FLIP_TARGET (DRM_MODE_PAGE_FLIP_TARGET_ABSOLUTE | DRM_MODE_PAGE_FLIP_TARGET_RELATIVE)
#define DRM_MODE_PAGE_FLIP_FLAGS (DRM_MODE_PAGE_FLIP_EVENT | DRM_MODE_PAGE_FLIP_ASYNC | DRM_MODE_PAGE_FLIP_TARGET)

/*
 * Request a page flip on the specified crtc.
 *
 * This ioctl will ask KMS to schedule a page flip for the specified
 * crtc. Once any pending rendering targeting the specified fb (as of
 * ioctl time) has completed, the crtc will be reprogrammed to display
 * that fb after the next vertical refresh. The ioctl returns
 * immediately, but subsequent rendering to the current fb will block
 * in the execbuffer ioctl until the page flip happens. If a page
 * flip is already pending as the ioctl is called, EBUSY will be
 * returned.
 *
 * Flag DRM_MODE_PAGE_FLIP_EVENT requests that drm sends back a vblank
 * event (see drm.h: struct drm_event_vblank) when the page flip is
 * done. The user_data field passed in with this ioctl will be
 * returned as the user_data field in the vblank event struct.
 *
 * Flag DRM_MODE_PAGE_FLIP_ASYNC requests that the flip happen
 * 'as soon as possible', meaning that it not delay waiting for vblank.
 * This may cause tearing on the screen.
 *
 * The reserved field must be zero.
 */

struct drm_mode_crtc_page_flip {
    uint32_t crtc_id;
    uint32_t fb_id;
    uint32_t flags;
    uint32_t reserved;
    uint64_t user_data;
};

// create a dumb scanout buffer
struct drm_mode_create_dumb {
    uint32_t height;
    uint32_t width;
    uint32_t bpp;
    uint32_t flags;
    // handle, pitch, size will be returned
    uint32_t handle;
    uint32_t pitch;
    uint64_t size;
};

// set up for mmap of a dumb scanout buffer
struct drm_mode_map_dumb {
    // Handle for the object being mapped.
    uint32_t handle;
    uint32_t pad;

    // Fake offset to use for subsequent mmap call
    uint64_t offset;
};

struct drm_mode_destroy_dumb {
    uint32_t handle;
};

// drm_connector.h
enum drm_connector_status {
    DRM_CONNECTOR_STATUS_CONNECTED = 1,
    DRM_CONNECTOR_STATUS_DISCONNECTED = 2,
    DRM_CONNECTOR_STATUS_UNKNOWN = 3,
};

// drm_fourcc.h
#define fourcc_code(a, b, c, d) ((uint32_t)(a) | ((uint32_t)(b) << 8) | ((uint32_t)(c) << 16) | ((uint32_t)(d) << 24))
#define DRM_FORMAT_XRGB8888 fourcc_code('X', 'R', '2', '4') /* [31:0] x:R:G:B 8:8:8:8 little endian */
