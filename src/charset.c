/*
|  Copyright (C) 2002 Jorg Schuler <jcsjcs at sourceforge.net>
|  Part of the gtkpod project.
| 
|  URL: 
| 
|  This program is free software; you can redistribute it and/or modify
|  it under the terms of the GNU General Public License as published by
|  the Free Software Foundation; either version 2 of the License, or
|  (at your option) any later version.
| 
|  This program is distributed in the hope that it will be useful,
|  but WITHOUT ANY WARRANTY; without even the implied warranty of
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
|  GNU General Public License for more details.
| 
|  You should have received a copy of the GNU General Public License
|  along with this program; if not, write to the Free Software
|  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
| 
|  iTunes and iPod are trademarks of Apple
| 
|  This product is not supported/written/published by Apple!
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include "prefs.h"
#include "charset.h"
#include "support.h"

const CharsetInfo charset_info[] = { 
    {N_("Arabic (IBM-864)"),                  "IBM864"        },
    {N_("Arabic (ISO-8859-6)"),               "ISO-8859-6"    },
    {N_("Arabic (Windows-1256)"),             "windows-1256"  },
    {N_("Baltic (ISO-8859-13)"),              "ISO-8859-13"   },
    {N_("Baltic (ISO-8859-4)"),               "ISO-8859-4"    },
    {N_("Baltic (Windows-1257)"),             "windows-1257"  },
    {N_("Celtic (ISO-8859-14)"),              "ISO-8859-14"   },
    {N_("Central European (IBM-852)"),        "IBM852"        },
    {N_("Central European (ISO-8859-2)"),     "ISO-8859-2"    },
    {N_("Central European (Windows-1250)"),   "windows-1250"  },
    {N_("Chinese Simplified (GB18030)"),      "gb18030"       },
    {N_("Chinese Simplified (GB2312)"),       "GB2312"        },
    {N_("Chinese Traditional (Big5)"),        "Big5"          },
    {N_("Chinese Traditional (Big5-HKSCS)"),  "Big5-HKSCS"    },
    {N_("Cyrillic (IBM-855)"),                "IBM855"        },
    {N_("Cyrillic (ISO-8859-5)"),             "ISO-8859-5"    },
    {N_("Cyrillic (ISO-IR-111)"),             "ISO-IR-111"    },
    {N_("Cyrillic (KOI8-R)"),                 "KOI8-R"        },
    {N_("Cyrillic (Windows-1251)"),           "windows-1251"  },
    {N_("Cyrillic/Russian (CP-866)"),         "IBM866"        },
    {N_("Cyrillic/Ukrainian (KOI8-U)"),       "KOI8-U"        },
    {N_("English (US-ASCII)"),                "us-ascii"      },
    {N_("Greek (ISO-8859-7)"),                "ISO-8859-7"    },
    {N_("Greek (Windows-1253)"),              "windows-1253"  },
    {N_("Hebrew (IBM-862)"),                  "IBM862"        },
    {N_("Hebrew (Windows-1255)"),             "windows-1255"  },
    {N_("Japanese (EUC-JP)"),                 "EUC-JP"        },
    {N_("Japanese (ISO-2022-JP)"),            "ISO-2022-JP"   },
    {N_("Japanese (Shift_JIS)"),              "Shift_JIS"     },
    {N_("Korean (EUC-KR)"),                   "EUC-KR"        },
    {N_("Nordic (ISO-8859-10)"),              "ISO-8859-10"   },
    {N_("South European (ISO-8859-3)"),       "ISO-8859-3"    },
    {N_("Thai (TIS-620)"),                    "TIS-620"       },
    {N_("Turkish (IBM-857)"),                 "IBM857"        },
    {N_("Turkish (ISO-8859-9)"),              "ISO-8859-9"    },
    {N_("Turkish (Windows-1254)"),            "windows-1254"  },
    {N_("Unicode (UTF-7)"),                   "UTF-7"         },
    {N_("Unicode (UTF-8)"),                   "UTF-8"         },
    {N_("Unicode (UTF-16BE)"),                "UTF-16BE"      },
    {N_("Unicode (UTF-16LE)"),                "UTF-16LE"      },
    {N_("Unicode (UTF-32BE)"),                "UTF-32BE"      },
    {N_("Unicode (UTF-32LE)"),                "UTF-32LE"      },
    {N_("Vietnamese (VISCII)"),               "VISCII"        },
    {N_("Vietnamese (Windows-1258)"),         "windows-1258"  },
    {N_("Visual Hebrew (ISO-8859-8)"),        "ISO-8859-8"    },
    {N_("Western (IBM-850)"),                 "IBM850"        },
    {N_("Western (ISO-8859-1)"),              "ISO-8859-1"    },
    {N_("Western (ISO-8859-15)"),             "ISO-8859-15"   },
    {N_("Western (Windows-1252)"),            "windows-1252"  },
    /*
     * From this point, character sets aren't supported by iconv
     */
/*    {N_("Arabic (IBM-864-I)"),                "IBM864i"              },
    {N_("Arabic (ISO-8859-6-E)"),             "ISO-8859-6-E"         },
    {N_("Arabic (ISO-8859-6-I)"),             "ISO-8859-6-I"         },
    {N_("Arabic (MacArabic)"),                "x-mac-arabic"         },
    {N_("Armenian (ARMSCII-8)"),              "armscii-8"            },
    {N_("Central European (MacCE)"),          "x-mac-ce"             },
    {N_("Chinese Simplified (GBK)"),          "x-gbk"                },
    {N_("Chinese Simplified (HZ)"),           "HZ-GB-2312"           },
    {N_("Chinese Traditional (EUC-TW)"),      "x-euc-tw"             },
    {N_("Croatian (MacCroatian)"),            "x-mac-croatian"       },
    {N_("Cyrillic (MacCyrillic)"),            "x-mac-cyrillic"       },
    {N_("Cyrillic/Ukrainian (MacUkrainian)"), "x-mac-ukrainian"      },
    {N_("Farsi (MacFarsi)"),                  "x-mac-farsi"},
    {N_("Greek (MacGreek)"),                  "x-mac-greek"          },
    {N_("Gujarati (MacGujarati)"),            "x-mac-gujarati"       },
    {N_("Gurmukhi (MacGurmukhi)"),            "x-mac-gurmukhi"       },
    {N_("Hebrew (ISO-8859-8-E)"),             "ISO-8859-8-E"         },
    {N_("Hebrew (ISO-8859-8-I)"),             "ISO-8859-8-I"         },
    {N_("Hebrew (MacHebrew)"),                "x-mac-hebrew"         },
    {N_("Hindi (MacDevanagari)"),             "x-mac-devanagari"     },
    {N_("Icelandic (MacIcelandic)"),          "x-mac-icelandic"      },
    {N_("Korean (JOHAB)"),                    "x-johab"              },
    {N_("Korean (UHC)"),                      "x-windows-949"        },
    {N_("Romanian (MacRomanian)"),            "x-mac-romanian"       },
    {N_("Turkish (MacTurkish)"),              "x-mac-turkish"        },
    {N_("User Defined"),                      "x-user-defined"       },
    {N_("Vietnamese (TCVN)"),                 "x-viet-tcvn5712"      },
    {N_("Vietnamese (VPS)"),                  "x-viet-vps"           },
    {N_("Western (MacRoman)"),                "x-mac-roman"          },
    // charsets whithout posibly translatable names
    {"T61.8bit",                              "T61.8bit"             },
    {"x-imap4-modified-utf7",                 "x-imap4-modified-utf7"},
    {"x-u-escaped",                           "x-u-escaped"          },
    {"windows-936",                           "windows-936"          }
*/
    {NULL, NULL}
};



/* Sets up the charsets to choose from in the "combo". It presets the
   charset stored in cfg->charset (or "System Charset" if none is set
   there */
void charset_init_combo (GtkCombo *combo)
{
    G_CONST_RETURN gchar *current_charset;
    const CharsetInfo *ci;
   
    static GList *charsets = NULL; /* list with choices -- takes a while to
				     * initialize, so we only do it once */
    
    current_charset = prefs_get_charset ();
    if ((current_charset == NULL) || (strlen (current_charset) == 0))
    {
	current_charset = _("System Charset");
    }
    if (charsets == NULL)
    { /* set up list with charsets */
	charsets = g_list_append (charsets, _("System Charset"));
	/* now add all the charset descriptions in the list above */
	ci=charset_info;
	while (ci->descr != NULL)
	{
	    charsets = g_list_append (charsets, ci->descr);
	    ++ci;
	}
	/* now add all the charsets listed by "iconv -l" */
	/* FIXME! */
    }
    /* set pull down items */
    gtk_combo_set_popdown_strings (GTK_COMBO (combo), charsets); 
    /* set standard entry */
    gtk_entry_set_text (GTK_ENTRY (GTK_COMBO (combo)->entry), current_charset);
}


/* returns the charset name belonging to the description "descr"
 * chosen from the combo. Return "NULL" when it could not be found, or
 * if it is the System Default Charset (locale). You must g_free
 * the charset received (unlike g_get_charset ()) */
gchar *charset_from_description (gchar *descr)
{
    const CharsetInfo *ci;

    if (!descr) return NULL; /* sanity! */
    /* check for "System Charset" and return NULL */
    if (g_utf8_collate (descr, _("System Charset")) == 0)   return NULL;
    /* check if description matches one of the descriptions in
     * charset_info[], and if so, return the charset name */
    ci = charset_info;
    while (ci->descr != NULL)
    {
	if (g_utf8_collate (descr, ci->descr) == 0)
	{
	    return g_strdup (ci->name);
	}
	++ci;
    }
    /* OK, it was not in the charset_info[] list. Therefore it must be
     * from the "iconv -l" list. We just return a copy of it */
    return g_strdup (descr);
}


/* Convert "str" (in the charset specified in cfg->charset) to
 * utf8. If cfg->charset is NULL, "str" is assumed to be in the
 * current locale charset */
gchar *charset_to_utf8 (gchar *str)
{
    gchar *ret;

    /* use standard locale charset */
    if (!cfg->charset || !strlen (cfg->charset))
	return g_locale_to_utf8(str, -1, NULL, NULL, NULL);
    /* OK, we have to use iconv */
    ret = g_convert (str,                  /* string to convert */
		     strlen (str),         /* length of string  */
		     "UTF-8",              /* to_codeset        */
		     prefs_get_charset (), /* from_codeset      */
		     NULL,                 /* *bytes_read       */
		     NULL,                 /* *bytes_written    */
		     NULL);                /* GError **error    */
    if (ret == NULL)  ret = g_strdup (_("Invalid Input String"));
    return ret;
}


/* Convert "str" from utf8 to the charset specified in
 * cfg->charset. If cfg->charset is NULL, "str" is converted to the
 * current locale charset */
gchar *charset_from_utf8 (gchar *str)
{
    gchar *ret;

    /* use standard locale charset */
    if (!cfg->charset || !strlen (cfg->charset))
	return g_locale_from_utf8(str, -1, NULL, NULL, NULL);
    /* OK, we have to use iconv */
    ret = g_convert (str,                  /* string to convert */
		    strlen (str),         /* length of string  */
		    prefs_get_charset (), /* to_codeset        */
		    "UTF-8",              /* from_codeset      */
		    NULL,                 /* *bytes_read       */
		    NULL,                 /* *bytes_written    */
		    NULL);                /* GError **error    */
    if (ret == NULL)  ret = g_strdup (_("Invalid Input String"));
    return ret;
}
