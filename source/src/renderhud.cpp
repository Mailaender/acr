// renderhud.cpp: HUD rendering

#include "cube.h"

void drawicon(Texture *tex, float x, float y, float s, int col, int row, float ts)
{
    if(tex && tex->xs == tex->ys) quad(tex->id, x, y, s, ts*col, ts*row, ts);
}

inline void turn_on_transparency(int alpha = 255)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4ub(255, 255, 255, alpha);
}

void drawequipicon(float x, float y, int col, int row, bool blend)
{
    static Texture *tex = NULL;
    if(!tex) tex = textureload("packages/misc/items.png", 4);
    if(tex)
    {
        turn_on_transparency();
        drawicon(tex, x, y, 120, col, row, 1/5.0f);
        glDisable(GL_BLEND);
    }
}

VARP(radarentsize, 4, 12, 64);

void drawradaricon(float x, float y, float s, int col, int row)
{
    static Texture *tex = NULL;
    if(!tex) tex = textureload("packages/misc/radaricons.png", 3);
    if(tex)
    {
        glEnable(GL_BLEND);
        drawicon(tex, x, y, s, col, row, 1/4.0f);
        glDisable(GL_BLEND);
    }
}

void drawflagicons(const flaginfo &f, playerent *p)
{
    static Texture *ctftex = textureload("packages/misc/ctficons.png", 3),
                   *hktftex = textureload("packages/misc/hktficons.png", 3),
                   *flagtex = textureload("packages/misc/flagicons.png", 3);
    if (flagtex)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1, 1, 1,
            f.state == CTFF_INBASE ? .2f :
            f.state == CTFF_IDLE ? .1f :
            f.actor == p && f.state == CTFF_STOLEN ? (sinf(lastmillis / 100.0f) + 1.0f) / 2.0f :
            1
            );
        // CTF
        int row = 0;
        // HTF
        if (m_hunt(gamemode)) row = 1;
        // KTF
        else if (m_keep(gamemode)) row = 2;
        drawicon(flagtex, f.team * 120 + VIRTW / 4.0f*3.0f, 1650, 120, f.team, row, 1 / 3.f);
    }
    // Must be stolen for big flag-stolen icon
    if (f.state != CTFF_STOLEN) return;
    Texture *t = (m_capture(gamemode) || (m_ktf2(gamemode, mutators) && m_team(gamemode, mutators))) ? ctftex : hktftex;
    if (!t) return;
    // CTF OR KTF2/Returner
    int row = (m_capture(gamemode) || (m_ktf2(gamemode, mutators) && m_team(gamemode, mutators))) && f.actor && f.actor->team == f.team ? 1 : 0;
    // HTF + KTF
    if (m_keep(gamemode) && !(m_ktf2(gamemode, mutators) && m_team(gamemode, mutators))) row = 1;
    // pulses
    glColor4f(1, 1, 1, f.actor == p ? (sinf(lastmillis / 100.0f) + 1.0f) / 2.0f : .6f);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    drawicon(t, VIRTW - 225 * (!f.team && flaginfos[1].state != CTFF_STOLEN ? 1 : 2 - f.team) - 10, VIRTH * 5 / 8, 225, f.team, row, 1 / 2.f);
}

void drawvoteicon(float x, float y, int col, int row, bool noblend)
{
    static Texture *tex = NULL;
    if(!tex) tex = textureload("packages/misc/voteicons.png", 3);
    if(tex)
    {
        if(noblend) glDisable(GL_BLEND);
        // else turn_on_transparency(); // if(transparency && !noblend)
        drawicon(tex, x, y, 240, col, row, 1/2.0f);
        if(noblend) glEnable(GL_BLEND);
    }
}

VARP(crosshairsize, 0, 15, 50);
VARP(showstats, 0, 1, 2);
VARP(crosshairfx, 0, 1, 1);
VARP(crosshairteamsign, 0, 1, 1);
VARP(hideradar, 0, 0, 1);
VARP(hidecompass, 0, 0, 1);
VARP(hideteam, 0, 0, 1);
VARP(hidectfhud, 0, 0, 1); // hardcore doesn't override
VARP(hidevote, 0, 0, 2);
VARP(hidehudmsgs, 0, 0, 1);
VARP(hidehudequipment, 0, 0, 1);
VARP(hidehudtarget, 0, 0, 1);
VARP(hideconsole, 0, 0, 1);
VARP(hideobits, 0, 0, 1);
VARP(hidespecthud, 0, 0, 1); // hardcore doesn't override
VARP(hidehardcore, 0, 2, 6);
VAR(showmap, 0, 0, 1);
#define show_hud_element(setting, hardcorelevel) (setting && (!m_real(gamemode, mutators) || hidehardcore < hardcorelevel))
#define hud_must_not_override(setting) setting // this is purely a decorator


//shotty::
/*
VAR(showsgpat, 0, 0, 1);

void drawsgpat(int w, int h)
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_TEXTURE_2D);
    glColor3ub(0, 0, 0);
    float sz = min(VIRTW, VIRTH),
    x1 = VIRTW/2 - sz/2,
    x2 = VIRTW/2 + sz/2,
    y1 = VIRTH/2 - sz/2,
    y2 = VIRTH/2 + sz/2,
    border = (512 - 64*2)/512.0f;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x1 + 0.5f*sz, y1 + 0.5f*sz);
    int rgbcv = 0;
    loopi(8+1)
    {
        // if((i%3)==0) { glColor3ub(rgbcv,rgbcv,rgbcv); rgbcv += 4; //rgbcv -= 255/(8+1); }
        if(i%2) glColor3ub(64,64,64); else glColor3ub(32,32,32);
        float c = 0.5f*(1 + border*cosf(i*2*M_PI/8.0f)), s = 0.5f*(1 + border*sinf(i*2*M_PI/8.0f));
        glVertex2f(x1 + c*sz, y1 + s*sz);
    }
    glColor3ub(255,255,255);
    glEnd();

    glDisable(GL_BLEND);

    rgbcv = 32;
    glBegin(GL_TRIANGLE_STRIP);
    loopi(8+1)
    {
        // if((i%3)==0) { glColor3ub(rgbcv,rgbcv,rgbcv); //,128); rgbcv += 8; //rgbcv -= 255/(8+1); }
        if(i%2) glColor3ub(16,16,16); else glColor3ub(32,32,32);
        float c = 0.5f*(1 + border*cosf(i*2*M_PI/8.0f)), s = 0.5f*(1 + border*sinf(i*2*M_PI/8.0f));
        glVertex2f(x1 + c*sz, y1 + s*sz);
        c = c < 0.4f ? 0 : (c > 0.6f ? 1 : 0.5f);
        s = s < 0.4f ? 0 : (s > 0.6f ? 1 : 0.5f);
        glVertex2f(x1 + c*sz, y1 + s*sz);
    }
    glColor3ub(255,255,255);
    glEnd();

    glEnable(GL_TEXTURE_2D);
    static Texture *pattex = NULL;
    if(!pattex) pattex = textureload("packages/misc/sgpat.png", 4);
    loopk(3)
    {
        switch(k)
        {
            case 0:  glColor3ub(  32, 250, 250); break; // center
            case 1:  glColor3ub( 250,  64,  64); break; // middle
            case 2:  glColor3ub( 250, 250,  64); break; // outer
            default: glColor3ub( 255, 255, 255); break;
        }
        extern sgray pat[SGRAYS*3];
        int j = k * SGRAYS;
        loopi(SGRAYS)
        {
            if(pattex)
            {
                vec p = pat[j+i].rv;
                int ppx = VIRTW/2 + p.x*(sz/2);
                int ppy = VIRTH/2 + p.y*(sz/2);
                drawicon(pattex, ppx, ppy, 16, 1, 1, 1);
            }
        }
    }
    glEnable(GL_BLEND);
    /\*
     // 2011may31: dmg/hits output comes upon each shot, let the pattern be shown "pure"
     extern int lastsgs_hits;
     extern int lastsgs_dmgt;
     //draw_textf("H: %d DMG: %d", 8, 32, lastsgs_hits, lastsgs_dmgt);
     defformatstring(t2show4hitdmg)("H: %d DMG: %d", lastsgs_hits, lastsgs_dmgt);
     draw_text(t2show4hitdmg, VIRTW/2-text_width(t2show4hitdmg), VIRTH/2-3*FONTH/4);
     *\/
}
*/
//::shotty

void drawscope(bool preload)
{
    static Texture *scopetex = NULL;
    if(!scopetex) scopetex = textureload("packages/misc/scope.png", 3);
    if(preload) return;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, scopetex->id);
    glColor3ub(255, 255, 255);

    // figure out the bounds of the scope given the desired aspect ratio
    float sz = min(VIRTW, VIRTH),
          x1 = VIRTW/2 - sz/2,
          x2 = VIRTW/2 + sz/2,
          y1 = VIRTH/2 - sz/2,
          y2 = VIRTH/2 + sz/2,
          border = (512 - 64*2)/512.0f;

    // draw center viewport
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0.5f, 0.5f);
    glVertex2f(x1 + 0.5f*sz, y1 + 0.5f*sz);
    loopi(8+1)
    {
        float c = 0.5f*(1 + border*cosf(i*2*M_PI/8.0f)), s = 0.5f*(1 + border*sinf(i*2*M_PI/8.0f));
        glTexCoord2f(c, s);
        glVertex2f(x1 + c*sz, y1 + s*sz);
    }
    glEnd();

    glDisable(GL_BLEND);

    // draw outer scope
    glBegin(GL_TRIANGLE_STRIP);
    loopi(8+1)
    {
        float c = 0.5f*(1 + border*cosf(i*2*M_PI/8.0f)), s = 0.5f*(1 + border*sinf(i*2*M_PI/8.0f));
        glTexCoord2f(c, s);
        glVertex2f(x1 + c*sz, y1 + s*sz);
        c = c < 0.4f ? 0 : (c > 0.6f ? 1 : 0.5f);
        s = s < 0.4f ? 0 : (s > 0.6f ? 1 : 0.5f);
        glTexCoord2f(c, s);
        glVertex2f(x1 + c*sz, y1 + s*sz);
    }
    glEnd();

    // fill unused space with border texels
    if(x1 > 0 || x2 < VIRTW || y1 > 0 || y2 < VIRTH)
    {
        glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0, 0); glVertex2f(0,  0);
        glTexCoord2f(0, 0); glVertex2f(x1, y1);
        glTexCoord2f(0, 1); glVertex2f(0,  VIRTH);
        glTexCoord2f(0, 1); glVertex2f(x1, y2);

        glTexCoord2f(1, 1); glVertex2f(VIRTW, VIRTH);
        glTexCoord2f(1, 1); glVertex2f(x2, y2);
        glTexCoord2f(1, 0); glVertex2f(VIRTW, 0);
        glTexCoord2f(1, 0); glVertex2f(x2, y1);

        glTexCoord2f(0, 0); glVertex2f(0,  0);
        glTexCoord2f(0, 0); glVertex2f(x1, y1);
        glEnd();
    }

    glEnable(GL_BLEND);
}

const char *crosshairnames[CROSSHAIR_NUM] = { "default", "scope", "shotgun", "v", "h", "hit", "reddot" };
Texture *crosshairs[CROSSHAIR_NUM] = { NULL }; // weapon specific crosshairs

Texture *loadcrosshairtexture(const char *c)
{
    defformatstring(p)("packages/crosshairs/%s", c);
    Texture *crosshair = textureload(p, 3);
    if(crosshair==notexture) crosshair = textureload("packages/crosshairs/default.png", 3);
    return crosshair;
}

void loadcrosshair(char *c, char *name)
{
    if (strcmp(name, "") == 0 || strcmp(name, "all") == 0)
    {
        for (int i = 0; i < CROSSHAIR_NUM; i++)
        {
            if (i == CROSSHAIR_SCOPE) continue;
            crosshairs[i] = loadcrosshairtexture(c);
        }
        return;
    }

    int n = -1;

    for (int i = 0; i < CROSSHAIR_NUM; i++)
    {
       if(strcmp(crosshairnames[i], name) == 0) { n = i; break; }
    }

    if (n < 0 || n >= CROSSHAIR_NUM) return;

    crosshairs[n] = loadcrosshairtexture(c);
}

COMMAND(loadcrosshair, "ss");

void drawcrosshair(playerent *p, int n, int teamtype)
{
    if (!show_hud_element(crosshairsize, 2) || intermission)
        return;

    Texture *crosshair = crosshairs[n];
    if(!crosshair)
    {
        crosshair = crosshairs[CROSSHAIR_DEFAULT];
        if(!crosshair) crosshair = crosshairs[CROSSHAIR_DEFAULT] = loadcrosshairtexture("default.png");
    }

    color col = color(1, 1, 1); // default: white
    if (teamtype)
    {
        if (teamtype == 1) col = color(0, 1, 0); // green
        else if (teamtype == 2) col = color(1, 0, 0); // red
    }
    else if (crosshairfx && !m_insta(gamemode, mutators))
    {
        if (p->health <= 50 * HEALTHSCALE) col = color(.5f, .25f, 0); // orange-red
        if (p->health <= 25 * HEALTHSCALE) col = color(.5f, .125f, 0); // red-orange
    }
    //if (n == CROSSHAIR_DEFAULT) col.alpha = 1.f - p->weaponsel->dynspread() / 1200.f;
    if (n != CROSSHAIR_SCOPE && p->zoomed) col.alpha = 1 - sqrtf(p->zoomed * (n == CROSSHAIR_SHOTGUN ? 0.5f : 1) * 1.6f);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4f(col.r, col.g, col.b, col.alpha * 0.8f);
    float chsize = (n == CROSSHAIR_SCOPE) ? 24.f : crosshairsize;
    if (n == CROSSHAIR_DEFAULT)
    {
        float clen = chsize * 3.6f;
        float cthickness = chsize * 2.f;
        chsize = p->weaponsel->dynspread() * 100 * (p->perk2 == PERK2_STEADY ? .65f : 1) / dynfov();
        //if (isthirdperson) chsize *= worldpos.dist(focus->o) / worldpos.dist(camera1->o);
        if (m_classic(gamemode, mutators)) chsize *= .6f;

        Texture *cv = crosshairs[CROSSHAIR_V], *ch = crosshairs[CROSSHAIR_H];
        if (!cv) cv = textureload("packages/crosshairs/vertical.png", 3);
        if (!ch) ch = textureload("packages/crosshairs/horizontal.png", 3);

        // horizontal
        glBindTexture(GL_TEXTURE_2D, ch->id);
        glBegin(GL_QUADS);
        // top
        glTexCoord2f(0, 0); glVertex2f(VIRTW / 2 - cthickness, VIRTH / 2 - chsize - clen);
        glTexCoord2f(1, 0); glVertex2f(VIRTW / 2 + cthickness, VIRTH / 2 - chsize - clen);
        glTexCoord2f(1, 1); glVertex2f(VIRTW / 2 + cthickness, VIRTH / 2 - chsize);
        glTexCoord2f(0, 1); glVertex2f(VIRTW / 2 - cthickness, VIRTH / 2 - chsize);
        // bottom
        glTexCoord2f(1, 1); glVertex2f(VIRTW / 2 - cthickness, VIRTH / 2 + chsize);
        glTexCoord2f(0, 1); glVertex2f(VIRTW / 2 + cthickness, VIRTH / 2 + chsize);
        glTexCoord2f(0, 0); glVertex2f(VIRTW / 2 + cthickness, VIRTH / 2 + chsize + clen);
        glTexCoord2f(1, 0); glVertex2f(VIRTW / 2 - cthickness, VIRTH / 2 + chsize + clen);
        glEnd();

        // vertical
        glBindTexture(GL_TEXTURE_2D, cv->id);
        glBegin(GL_QUADS);
        // left
        glTexCoord2f(0, 0); glVertex2f(VIRTW / 2 - chsize - clen, VIRTH / 2 - cthickness);
        glTexCoord2f(1, 0); glVertex2f(VIRTW / 2 - chsize, VIRTH / 2 - cthickness);
        glTexCoord2f(1, 1); glVertex2f(VIRTW / 2 - chsize, VIRTH / 2 + cthickness);
        glTexCoord2f(0, 1); glVertex2f(VIRTW / 2 - chsize - clen, VIRTH / 2 + cthickness);
        // right
        glTexCoord2f(1, 1); glVertex2f(VIRTW / 2 + chsize, VIRTH / 2 - cthickness);
        glTexCoord2f(0, 1); glVertex2f(VIRTW / 2 + chsize + clen, VIRTH / 2 - cthickness);
        glTexCoord2f(0, 0); glVertex2f(VIRTW / 2 + chsize + clen, VIRTH / 2 + cthickness);
        glTexCoord2f(1, 0); glVertex2f(VIRTW / 2 + chsize, VIRTH / 2 + cthickness);
        glEnd();
    }
    else
    {
        if (n == CROSSHAIR_SHOTGUN)
        {
            chsize = p->weaponsel->dynspread() * 100 * (p->perk2 == PERK2_STEADY ? .75f : 1) / dynfov();
            //if (isthirdperson) chsize *= worldpos.dist(focus->o) / worldpos.dist(camera1->o);
            if (m_classic(gamemode, mutators)) chsize *= .75f;
        }

        glBindTexture(GL_TEXTURE_2D, crosshair->id);
        glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0, 0); glVertex2f(VIRTW / 2 - chsize, VIRTH / 2 - chsize);
        glTexCoord2f(1, 0); glVertex2f(VIRTW / 2 + chsize, VIRTH / 2 - chsize);
        glTexCoord2f(0, 1); glVertex2f(VIRTW / 2 - chsize, VIRTH / 2 + chsize);
        glTexCoord2f(1, 1); glVertex2f(VIRTW / 2 + chsize, VIRTH / 2 + chsize);
        glEnd();
    }
}

VARP(hitmarkerfade, 0, 750, 5000);

void drawhitmarker()
{
    if (!show_hud_element(hitmarkerfade, 3) || !focus->lasthit || focus->lasthit + hitmarkerfade <= lastmillis)
        return;

    glColor4f(1, 1, 1, (focus->lasthit + hitmarkerfade - lastmillis) / 1000.f);
    Texture *ch = crosshairs[CROSSHAIR_HIT];
    if (!ch) ch = textureload("packages/crosshairs/hit.png", 3);
    if (ch->bpp == 32) glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    else glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glBindTexture(GL_TEXTURE_2D, ch->id);
    glBegin(GL_TRIANGLE_STRIP);
    const float hitsize = 56.f;
    glTexCoord2f(0, 0); glVertex2f(VIRTW / 2 - hitsize, VIRTH / 2 - hitsize);
    glTexCoord2f(1, 0); glVertex2f(VIRTW / 2 + hitsize, VIRTH / 2 - hitsize);
    glTexCoord2f(0, 1); glVertex2f(VIRTW / 2 - hitsize, VIRTH / 2 + hitsize);
    glTexCoord2f(1, 1); glVertex2f(VIRTW / 2 + hitsize, VIRTH / 2 + hitsize);
    glEnd();
}

VARP(hidedamageindicator, 0, 0, 1);
VARP(damageindicatorsize, 0, 200, 10000);
VARP(damageindicatordist, 0, 500, 10000);
VARP(damageindicatortime, 1, 2000, 10000);
VARP(damageindicatoralpha, 1, 50, 100);

void drawdmgindicator()
{
    if (!damageindicatorsize || !focus->damagestack.length()) return;

    static Texture *damagedirtex = NULL;
    if (!damagedirtex) damagedirtex = textureload("packages/misc/damagedir.png", 3);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, damagedirtex->id);

    loopv(focus->damagestack)
    {
        const damageinfo &pain = focus->damagestack[i];
        const float damagefade = damageindicatortime + pain.damage * 200 / HEALTHSCALE;
        if (pain.millis + damagefade <= lastmillis)
        {
            focus->damagestack.remove(i--);
            continue;
        }
        vec dir = pain.o;
        dir.sub(focus->o).normalize();
        const float fade = (1 - (lastmillis - pain.millis) / damagefade) * damageindicatoralpha / 100.f,
            dirangle = dir.x ? atan2f(dir.y, dir.x) / RAD : dir.y < 0 ? 270 : 90;
        glPushMatrix();
        glTranslatef(VIRTW / 2, VIRTH / 2, 0);
        glRotatef(dirangle + 90 - player1->yaw, 0, 0, 1);
        glTranslatef(0, -damageindicatordist, 0);
        glColor4f(1, 1, 1, fade);

        glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0, 0); glVertex2f(-damageindicatorsize, -damageindicatorsize / 2);
        glTexCoord2f(1, 0); glVertex2f( damageindicatorsize, -damageindicatorsize / 2);
        glTexCoord2f(0, 1); glVertex2f(-damageindicatorsize,  damageindicatorsize / 2);
        glTexCoord2f(1, 1); glVertex2f( damageindicatorsize,  damageindicatorsize / 2);
        glEnd();
        glPopMatrix();
    }
}

void drawequipicons(playerent *p)
{
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 1.0f, 0.2f+(sinf(lastmillis/100.0f)+1.0f)/2.0f);

    // health & armor
    if (show_hud_element(!hidehudequipment, 1))
    {
        int hc = 0, hr = 3;
        if (p->armour)
        {
            hr = 2;
            if (p->armour >= 100) hc = 4;
            else if (p->armour >= 75) hc = 3;
            else if (p->armour >= 50) hc = 2;
            else if (p->armour >= 25) hc = 1;
            else hc = 0;
        }
        drawequipicon(20, 1650, hc, hr, (p->state != CS_DEAD && p->health <= 35 * HEALTHSCALE && m_regen(gamemode, mutators)));
    }

    // grenades and throwing knives
    int equipx = 0;
    loopi(min(3, p->mag[GUN_GRENADE])) drawequipicon(1020 + equipx++ * 25, 1650, 1, 1, false);
    loopi(min(3, p->ammo[GUN_KNIFE])) drawequipicon(1060 + equipx++ * 30, 1650, 0, 0, false);

    // weapons
    if (p->weaponsel && p->weaponsel->type >= GUN_KNIFE && p->weaponsel->type < NUMGUNS)
    {
        int c = p->weaponsel->type, r = 0;
        if (c == GUN_GRENADE)
        {
            // draw nades separately
            if (p->prevweaponsel && p->prevweaponsel->type != GUN_GRENADE) c = p->prevweaponsel->type;
            else if (p->nextweaponsel && p->nextweaponsel->type != GUN_GRENADE) c = p->nextweaponsel->type;
            else c = 14; // unknown = HP symbol
        }
        switch (c){
            case GUN_KNIFE: case GUN_PISTOL: case GUN_SHOTGUN: case GUN_SUBGUN: break; // aligned properly
            default: c = 0; break;
            case GUN_SWORD: c = 0; r = 0; break; // special: sword uses knife
            case GUN_SNIPER: case GUN_BOLT: case GUN_SNIPER2: c = 4; r = 0; break; // special: snipers are shared
            case GUN_ASSAULT: c = 0; r = 1; break;
            case GUN_GRENADE: c = 1; r = 1; break;
            case GUN_HEAL: c = 2; r = 1; break;
            case GUN_RPG: c = 3; r = 1; break;
            case GUN_ASSAULT2: c = 4; r = 1; break;
            case GUN_AKIMBO: c = GUN_PISTOL; break; // special: pistol and akimbo share
        }
        drawequipicon(560, 1650, c, r, ((!p->weaponsel->ammo || p->weaponsel->mag < magsize(p->weaponsel->type) / 3) && !melee_weap(p->weaponsel->type) && p->weaponsel->type != GUN_GRENADE));
    }
    glEnable(GL_BLEND);
}

void drawradarent(float x, float y, float yaw, int col, int row, float iconsize, int pulse = 0, float alpha = 1.0f, const char *label = NULL, ...)
{
    glPushMatrix();
    if(pulse) glColor4f(1.0f, 1.0f, 1.0f, 0.2f+(sinf(lastmillis/30.0f+pulse)+1.0f)/2.0f);
    else glColor4f(1, 1, 1, alpha);
    glTranslatef(x, y, 0);
    glRotatef(yaw, 0, 0, 1);
    drawradaricon(-iconsize/2.0f, -iconsize/2.0f, iconsize, col, row);
    glPopMatrix();
    if(label && showmap)
    {
        glPushMatrix();
        glEnable(GL_BLEND);
        glTranslatef(iconsize/2, iconsize/2, 0);
        glScalef(1/2.0f, 1/2.0f, 1/2.0f);
        defvformatstring(lbl, label, label);
        draw_text(lbl, (int)(x * 2), (int)(y * 2), 255, 255, 255, int(alpha * 127.5f));
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_BLEND);
        glPopMatrix();
    }
}

struct hudline : cline
{
    int type;

    hudline() : type(HUDMSG_INFO) {}
};

struct hudmessages : consolebuffer<hudline>
{
    hudmessages() : consolebuffer<hudline>(20) {}

    void addline(const char *sf)
    {
        if(conlines.length() && conlines[0].type&HUDMSG_OVERWRITE)
        {
            conlines[0].millis = totalmillis;
            conlines[0].type = HUDMSG_INFO;
            copystring(conlines[0].line, sf);
        }
        else consolebuffer<hudline>::addline(sf, totalmillis);
    }
    void editline(int type, const char *sf)
    {
        if(conlines.length() && ((conlines[0].type&HUDMSG_TYPE)==(type&HUDMSG_TYPE) || conlines[0].type&HUDMSG_OVERWRITE))
        {
            conlines[0].millis = totalmillis;
            conlines[0].type = type;
            copystring(conlines[0].line, sf);
        }
        else consolebuffer<hudline>::addline(sf, totalmillis).type = type;
    }
    void render()
    {
        if(!conlines.length()) return;
        glPushMatrix();
        glLoadIdentity();
        int origVIRTW = VIRTW;
        glOrtho(0, origVIRTW*0.9f, VIRTH*0.9f, 0, -1, 1);
        glTranslatef((float)0.9f*origVIRTW*(monitors - 2 + (monitors&1))/(2.*monitors), 0., 0.);
        VIRTW /= (float)monitors/(float)(2 - (monitors & 1));
        int dispmillis = arenaintermission ? 6000 : 3000;
        loopi(min(conlines.length(), 3)) if(totalmillis-conlines[i].millis<dispmillis)
        {
            cline &c = conlines[i];
            int tw = text_width(c.line);
            draw_text(c.line, int(tw > VIRTW*0.9f ? 0 : (VIRTW*0.9f-tw)/2), int(((VIRTH*0.9f)/4*3)+FONTH*i+pow((totalmillis-c.millis)/(float)dispmillis, 4)*VIRTH*0.9f/4.0f));
        }
        glPopMatrix();
        VIRTW = origVIRTW;
    }
};

hudmessages hudmsgs;

void hudoutf(const char *s, ...)
{
    defvformatstring(sf, s, s);
    hudmsgs.addline(sf);
    conoutf("%s", sf);
}

void hudonlyf(const char *s, ...)
{
    defvformatstring(sf, s, s);
    hudmsgs.addline(sf);
}

void hudeditf(int type, const char *s, ...)
{
    defvformatstring(sf, s, s);
    hudmsgs.editline(type, sf);
}

bool insideradar(const vec &centerpos, float radius, const vec &o)
{
    if(showmap) return !o.reject(centerpos, radius);
    return o.distxy(centerpos)<=radius;
}

bool isattacking(playerent *p) { return lastmillis-p->lastaction < 500; }

vec getradarpos()
{
    float radarviewsize = VIRTH/6;
    float overlaysize = radarviewsize*4.0f/3.25f;
    return vec(VIRTW-10-VIRTH/28-overlaysize, 10+VIRTH/52, 0);
}

void DrawCircle(float cx, float cy, float r, GLubyte *col, float thickness = 1.f, int segments = 720)
{
    // Adapted from public domain code at http://slabode.exofire.net/circle_draw.shtml
    const float theta = 2 * PI / float(segments);
    const float c = cosf(theta); // precalculate the sine and cosine
    const float s = sinf(theta);

    float t;
    float x = r; // we start at angle = 0
    float y = 0;

    glEnable(GL_LINE_SMOOTH);
    glLineWidth(thickness);
    glBegin(GL_LINE_LOOP);
    glColor4ubv(col);
    loopi(segments)
    {
        glVertex2f(x + cx, y + cy); // output vertex
        // apply the rotation matrix
        t = x;
        x = c * x - s * y;
        y = s * t + c * y;
    }
    glEnd();
    glDisable(GL_LINE_SMOOTH);
}

VARP(showmapbackdrop, 0, 0, 2);
VARP(showmapbackdroptransparency, 0, 75, 100);
VARP(radarheight, 5, 150, 500);
VAR(showradarvalues, 0, 0, 1); // DEBUG
VARP(radarenemyfade, 0, 1250, 1250);

void drawradar_showmap(playerent *p, int w, int h)
{
    float minimapviewsize = 3*min(VIRTW,VIRTH)/4; //minimap default size
    float halfviewsize = minimapviewsize/2.0f;
    float iconsize = radarentsize/0.2f;
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    extern GLuint minimaptex;
    vec centerpos(VIRTW/2 , VIRTH/2, 0.0f);
    if(showmapbackdrop)
    {
        glDisable(GL_TEXTURE_2D);
        if(showmapbackdrop==2) glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE_MINUS_SRC_COLOR);
        loopi(2)
        {
            int cg = i?(showmapbackdrop==2?((int)(255*(100-showmapbackdroptransparency)/100.0f)):0):(showmapbackdrop==2?((int)(255*(100-showmapbackdroptransparency)/100.0f)):64);
            int co = i?0:4;
            glColor3ub(cg, cg, cg);
            glBegin(GL_QUADS);
            glVertex2f( centerpos.x - halfviewsize - co, centerpos.y + halfviewsize + co);
            glVertex2f( centerpos.x + halfviewsize + co, centerpos.y + halfviewsize + co);
            glVertex2f( centerpos.x + halfviewsize + co, centerpos.y - halfviewsize - co);
            glVertex2f( centerpos.x - halfviewsize - co, centerpos.y - halfviewsize - co);
            glEnd();
        }
        glColor3ub(255,255,255);
        glEnable(GL_TEXTURE_2D);
    }
    glTranslatef(centerpos.x - halfviewsize, centerpos.y - halfviewsize , 0);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
    quad(minimaptex, 0, 0, minimapviewsize, 0.0f, 0.0f, 1.0f);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);

    float gdim = max(mapdims[4], mapdims[5]); //no border
    float offd = fabs(float(mapdims[5])-float(mapdims[4])) /2.0f;
    if(!gdim) { gdim = ssize/2.0f; offd = 0; }
    float coordtrans = (minimapviewsize)/(gdim);

    float offx = gdim==mapdims[5] ? offd : 0;
    float offy = gdim==mapdims[4] ? offd : 0;

    vec mdd = vec(mapdims[0]-offx, mapdims[1]-offy, 0);
    vec cod(offx, offy, 0);
    vec ppv = vec(p->o).sub(mdd).mul(coordtrans);

    // local player
    drawradarent(ppv.x, ppv.y, p->yaw, p->state == CS_DEAD ? 1 : (isattacking(p) ? 2 : 0), 1, iconsize, isattacking(p) ? 1 : 0, p->state == CS_DEAD ? .5f : 1.f, "\f1%s", colorname(p));
    // other players
    const bool hasradar = radarup(p) || p->team == TEAM_SPECT;
    loopv(players)
    {
        playerent *pl = players[i];
        if(!pl || pl==p) continue;
        bool force = hasradar || // radar earned
            pl->state == CS_DEAD || // dead player
            isteam(p, pl); // same team
        if (force)
        {
            vec rtmp = vec(pl->o).sub(mdd).mul(coordtrans);
            drawradarent(rtmp.x, rtmp.y, pl->yaw, pl->state == CS_DEAD ? 1 : (isattacking(pl) ? 2 : 0), isteam(p, pl) ? 2 : 0, iconsize, isattacking(pl) ? 1 : 0, pl->team == TEAM_SPECT ? .2f : pl->state == CS_DEAD ? .5f : 1, "\f%d%s", pl->team == TEAM_SPECT ? 4 : isteam(p, pl) ? 0 : (p->team == TEAM_SPECT) ? team_color(pl->team) : 3, colorname(pl));
        }
        else if (pl->radarmillis + radarenemyfade >= lastmillis)
        {
            vec rtmp = vec(pl->lastloudpos.v).sub(mdd).mul(coordtrans);
            drawradarent(rtmp.x, rtmp.y, pl->lastloudpos.w, isattacking(pl) ? 2 : 0, isteam(p, pl) ? 2 : 0, iconsize, 0, (radarenemyfade - lastmillis + pl->radarmillis) / (float)radarenemyfade, "\f3%s", colorname(pl));
        }
    }
    if(m_flags(gamemode))
    {
        if (m_secure(gamemode))
        {
            loopv(ents)
            {
                entity &e = ents[i];
                if (e.type != CTF_FLAG || e.attr2 < 2 || OUTBORD(ents[i].x, ents[i].y))
                    continue;
                float overthrown = ents[i].attr4 / 255.f, owned = 1.f - overthrown;
                const int newteam = (ents[i].attr2 == TEAM_SPECT + 2 || m_gsp1(gamemode, mutators)) ? ents[i].attr3 : 2;
                // base only
                vec pos = vec(e.x, e.y, 0).sub(mdd).mul(coordtrans);
                drawradarent(pos.x, pos.y, 0, clamp((int)(ents[i].attr2 - 2), (int)TEAM_CLA, 2), 3, iconsize, 0, owned);
                drawradarent(pos.x, pos.y, 0, newteam, 3, iconsize, 0, overthrown);
            }
        }
        else
        {
            glColor4f(1.0f, 1.0f, 1.0f, (sinf(lastmillis / 100.0f) + 1.0f) / 2.0f);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            loopi(2) // flag items
            {
                flaginfo &f = flaginfos[i];
                entity *e = f.flagent;
                if(!e) continue;
                if(e->x == -1 && e-> y == -1) continue; // flagdummies
                vec pos = vec(e->x, e->y, 0).sub(mdd).mul(coordtrans);
                drawradarent(pos.x, pos.y, 0, m_keep(gamemode) && !m_ktf2(gamemode, mutators) && f.state != CTFF_IDLE ? 2 : f.team, 3, iconsize); // draw bases
                vec fltxoff = vec(8, -8, 0);
                vec cpos = vec(f.pos.x, f.pos.y, f.pos.z).sub(mdd).mul(coordtrans).add(fltxoff);
                if(f.state!=CTFF_STOLEN)
                {
                    float flgoff=fabs((radarentsize*2.1f)-8);
                    drawradarent(cpos.x+flgoff, cpos.y-flgoff, 0, 3, m_keep(gamemode) && !m_ktf2(gamemode, mutators) ? 2 : f.team, iconsize, 0, f.state == CTFF_IDLE ? .3f : 1); // draw on entity pos
                }
                if (f.state == CTFF_STOLEN && f.actor)
                {
                    float d2c = 1.6f * radarentsize / 16.0f;
                    vec apos(d2c, -d2c, 0);
                    if (hasradar || isteam(p, f.actor))
                        apos.add(f.actor->o);
                    else if (!m_classic(gamemode, mutators))
                        apos.add(f.actor->lastloudpos.v);
                    apos.sub(mdd).mul(coordtrans);
                    drawradarent(apos.x, apos.y, 0, 3, m_keep(gamemode) ? 2 : f.team, iconsize, f.team + 1); // draw near flag thief
                }
            }
        }
    }
    else if (m_edit(gamemode))
    {
        loopv(ents)
            if (ents[i].type == CTF_FLAG && !OUTBORD(ents[i].x, ents[i].y))
            {
                float d2c = 1.6f * radarentsize / 16.0f;
                vec apos(d2c, -d2c, 0);
                apos.x += ents[i].x;
                apos.y += ents[i].y;
                apos.sub(mdd).mul(coordtrans);
                drawradarent(apos.x, apos.y, 0, clamp((int)ents[i].attr2, 0, 2), 3, iconsize);
            }
    }
    loopv(bounceents) // grenades
    {
        bounceent *b = bounceents[i];
        if (!b || b->bouncetype != BT_NADE) continue;
        if (((grenadeent *)b)->nadestate != 1) continue;
        vec rtmp = vec(b->o).sub(mdd).mul(coordtrans);
        drawradarent(rtmp.x, rtmp.y, 0, b->owner == p ? 2 : isteam(b->owner, p) ? 1 : 0, 3, iconsize / 1.5f, 1);
    }
    glEnable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    loopv(radar_explosions) // explosions
    {
        int ndelay = lastmillis - radar_explosions[i].millis;
        if (ndelay > 600) radar_explosions.remove(i--);
        else
        {
            static GLubyte col_ownexp[4] = { 0xf7, 0xf5, 0x34 }; // yellow for your own explosions
            static GLubyte col_friendlyexp[4] = { 0x02, 0x13, 0xFB }; // blue for friendlies' explosions
            static GLubyte col_enemyexp[4] = { 0xFB, 0x02, 0x02 }; // red for enemies' explosions
            GLubyte *col;
            if (radar_explosions[i].owner == p) col = col_ownexp;
            else if (isteam(p, radar_explosions[i].owner)) col = col_friendlyexp;
            else col = col_enemyexp;
            vec nxpo(radar_explosions[i].o[0], radar_explosions[i].o[1], 0);
            nxpo.sub(mdd).mul(coordtrans);
            if (ndelay < 400)
            {
                col[3] = (1.f - ndelay / 400.f) * 255;
                DrawCircle(nxpo.x, nxpo.y, ndelay / 100.f * coordtrans, col, 2.f);
            }
            col[3] = (1.f - ndelay / 600.f) * 255;
            DrawCircle(nxpo.x, nxpo.y, pow(ndelay, 1.5f) / 3094.0923f * coordtrans, col);
        }
    }
    loopv(radar_shotlines) // shotlines
    {
        if (radar_shotlines[i].expire < lastmillis) radar_shotlines.remove(i--);
        else
        {
            static const GLubyte col_ownshot[3] = { 0x94, 0xB0, 0xDE }; // blue for your shots
            static const GLubyte col_friendlyshot[3] = { 0xB8, 0xDC, 0x78 }; // light green-yellow for friendlies
            static const GLubyte col_enemyshot[3] = { 0xFF, 0xFF, 0xFF }; // white for enemies
            const GLubyte *col;
            if (radar_shotlines[i].owner == p) col = col_ownshot;
            else if (isteam(p, radar_shotlines[i].owner)) col = col_friendlyshot;
            else col = col_enemyshot;
            glBegin(GL_LINES);
            vec from(radar_shotlines[i].from[0], radar_shotlines[i].from[1], 0), to(radar_shotlines[i].to[0], radar_shotlines[i].to[1], 0);
            from.sub(mdd);
            to.sub(mdd);
            // source shot
            glColor4ub(col[0], col[1], col[2], 200);
            glVertex2f(from.x*coordtrans, from.y*coordtrans);
            // dest shot
            glColor4ub(col[0], col[1], col[2], 250);
            glVertex2f(to.x*coordtrans, to.y*coordtrans);
            glEnd();
        }
    }
    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
}

void drawradar_vicinity(playerent *p, int w, int h)
{
    extern GLuint minimaptex;
    int gdim = max(mapdims[4], mapdims[5]);
    float radarviewsize = min(VIRTW,VIRTH)/5;
    float halfviewsize = radarviewsize/2.0f;
    float iconsize = radarentsize/0.4f;
    float scaleh = radarheight/(2.0f*gdim);
    float scaled = radarviewsize/float(radarheight);
    float offd = fabs((mapdims[5]-mapdims[4]) /2.0f);
    if(!gdim) { gdim = ssize/2; offd = 0; }
    float offx = gdim==mapdims[5]?offd:0;
    float offy = gdim==mapdims[4]?offd:0;
    vec rtr = vec(mapdims[0]-offx, mapdims[1]-offy, 0);
    vec rsd = vec(mapdims[0]+mapdims[4]/2, mapdims[1]+mapdims[5]/2, 0);
    float d2s = radarheight/2.0f*.8f;
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    vec centerpos(VIRTW-halfviewsize-72, halfviewsize+64, 0);
    glTranslatef(centerpos.x, centerpos.y, 0);
    glRotatef(-camera1->yaw, 0, 0, 1);
    glTranslatef(-halfviewsize, -halfviewsize, 0);
    vec d4rc = vec(p->o).sub(rsd).normalize().mul(0);
    vec usecenter = vec(p->o).sub(rtr).sub(d4rc);
    if(showradarvalues)
    {
        conoutf("vicinity @ gdim = %d | scaleh = %.2f", gdim, scaleh);
        conoutf("offd: %.2f [%.2f:%.2f]", offd, offx, offy);
        conoutf("RTR: %.2f %.2f", rtr.x, rtr.y);
        conoutf("RSD: %.2f %.2f", rsd.x, rsd.y);
        conoutf("P.O: %.2f %.2f", p->o.x, p->o.y);
        conoutf("U4C: %.2f %.2f | %.2f %.2f", usecenter.x, usecenter.y, usecenter.x/gdim, usecenter.y/gdim);
        //showradarvalues = 0;
    }
    glDisable(GL_BLEND);
    circle(minimaptex, halfviewsize, halfviewsize, halfviewsize, usecenter.x/(float)gdim, usecenter.y/(float)gdim, scaleh, 31); //Draw mimimaptext as radar background
    glTranslatef(halfviewsize, halfviewsize, 0);
    // local player
    drawradarent(0, 0, p->yaw, p->state == CS_DEAD ? 1 : (isattacking(p) ? 2 : 0), 1, iconsize, isattacking(p) ? 1 : 0, p->state == CS_DEAD ? .5f : 1.f, "\f1%s", colorname(p));
    const bool hasradar = radarup(p) || p->team == TEAM_SPECT;
    // other players
    loopv(players)
    {
        playerent *pl = players[i];
        if (!pl || pl == p) continue;
        bool force = hasradar || // radar earned
            pl->state == CS_DEAD || // dead player
            isteam(p, pl); // same team
        if (force)
        {
            vec rtmp = vec(pl->o).sub(p->o);
            if (rtmp.magnitude() > d2s)
            {
                if (pl->state == CS_DEAD)
                    continue;
                else
                    rtmp.normalize().mul(d2s);
            }
            rtmp.mul(scaled);
            drawradarent(rtmp.x, rtmp.y, pl->yaw, pl->state == CS_DEAD ? 1 : (isattacking(pl) ? 2 : 0), isteam(p, pl) ? 2 : 0, iconsize, isattacking(pl) ? 1 : 0, pl->team == TEAM_SPECT ? .2f : pl->state == CS_DEAD ? .5f : 1, "\f%d%s", pl->team == TEAM_SPECT ? 4 : isteam(p, pl) ? 0 : (p->team == TEAM_SPECT) ? team_color(pl->team) : 3, colorname(pl));
        }
        else if (pl->radarmillis + radarenemyfade >= lastmillis)
        {
            vec rtmp = vec(pl->lastloudpos.v).sub(p->o);
            if (rtmp.magnitude() > d2s)
                rtmp.normalize().mul(d2s);
            rtmp.mul(scaled);
            drawradarent(rtmp.x, rtmp.y, pl->lastloudpos.w, pl->state == CS_DEAD ? 1 : (isattacking(pl) ? 2 : 0), isteam(p, pl) ? 2 : 0, iconsize, 0, (radarenemyfade - lastmillis + pl->radarmillis) / (float)radarenemyfade, "\f3%s", colorname(pl));
        }
    }
    if (m_flags(gamemode))
    {
        if (m_secure(gamemode))
        {
            loopv(ents)
            {
                entity &e = ents[i];
                if (e.type != CTF_FLAG || e.attr2 < 2 || OUTBORD(ents[i].x, ents[i].y))
                    continue;
                float overthrown = ents[i].attr4 / 255.f, owned = 1.f - overthrown;
                const int newteam = (ents[i].attr2 == TEAM_SPECT + 2 || m_gsp1(gamemode, mutators)) ? ents[i].attr3 : 2;
                // base only
                vec pos = vec(e.x, e.y, 0).sub(p->o);
                if (pos.magnitude() > d2s)
                    pos.normalize().mul(d2s);
                pos.mul(scaled);
                drawradarent(pos.x, pos.y, 0, clamp((int)(ents[i].attr2 - 2), (int)TEAM_CLA, 2), 3, iconsize, 0, owned);
                drawradarent(pos.x, pos.y, 0, newteam, 3, iconsize, 0, overthrown);
            }
        }
        else
        {
            glColor4f(1.0f, 1.0f, 1.0f, (sinf(lastmillis / 100.0f) + 1.0f) / 2.0f);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            float d2c = 1.6f * radarentsize / 16.0f;
            loopi(2) // flag items
            {
                flaginfo &f = flaginfos[i];
                entity *e = f.flagent;
                if (!e) continue;
                if (e->x == -1 && e->y == -1) continue; // flagdummies
                vec pos = vec(e->x, e->y, 0).sub(p->o);
                vec cpos = vec(f.pos.x, f.pos.y, f.pos.z).sub(p->o);
                //if(showradarvalues) { conoutf("dist2F[%d]: %.2f|%.2f || %.2f|%.2f", i, pos.x, pos.y, cpos.x, cpos.y); }
                if (pos.magnitude() > d2s)
                    pos.normalize().mul(d2s);
                pos.mul(scaled);
                drawradarent(pos.x, pos.y, 0, m_keep(gamemode) && !m_ktf2(gamemode, mutators) && f.state != CTFF_IDLE ? 2 : f.team, 3, iconsize); // draw bases [circle doesn't need rotating]
                if (f.state != CTFF_STOLEN)
                {
                    if (cpos.magnitude() > d2s)
                        cpos.normalize().mul(d2s);
                    cpos.mul(scaled);
                    float flgoff = radarentsize / 0.68f;
                    float ryaw = (camera1->yaw - 45)*(2 * PI / 360);
                    float offx = flgoff*cosf(-ryaw);
                    float offy = flgoff*sinf(-ryaw);
                    drawradarent(cpos.x + offx, cpos.y - offy, camera1->yaw, 3, m_keep(gamemode) && !m_ktf2(gamemode, mutators) ? 2 : f.team, iconsize, 0, f.state == CTFF_IDLE ? .3f : 1); // draw flag on entity pos
                }
                if (f.state == CTFF_STOLEN && f.actor)
                {
                    vec apos(d2c, -d2c, 0);
                    if (hasradar || isteam(p, f.actor))
                        apos.add(f.actor->o);
                    else if (!m_classic(gamemode, mutators))
                        apos.add(f.actor->lastloudpos.v);
                    apos.sub(p->o);
                    if (apos.magnitude() > d2s)
                        apos.normalize().mul(d2s);
                    apos.mul(scaled);
                    drawradarent(apos.x, apos.y, camera1->yaw, 3, m_keep(gamemode) ? 2 : f.team, iconsize, f.team + 1); // draw near flag thief
                }
            }
        }
    }
    else if (m_edit(gamemode))
    {
        loopv(ents)
            if (ents[i].type == CTF_FLAG && !OUTBORD(ents[i].x, ents[i].y))
            {
                float d2c = 1.6f * radarentsize / 16.0f;
                vec apos(d2c, -d2c, 0);
                apos.x += ents[i].x;
                apos.y += ents[i].y;
                apos.sub(p->o);
                if (apos.magnitude() > d2s)
                    apos.normalize().mul(d2s);
                apos.mul(scaled);
                drawradarent(apos.x, apos.y, 0, clamp((int)ents[i].attr2, 0, 2), 3, iconsize);
            }
    }
    showradarvalues = 0; // DEBUG - also see two bits commented-out above
    loopv(bounceents) // grenades
    {
        bounceent *b = bounceents[i];
        if (!b || b->bouncetype != BT_NADE) continue;
        if (((grenadeent *)b)->nadestate != 1) continue;
        vec rtmp = vec(b->o).sub(p->o);
        if (rtmp.magnitude() > d2s)
            rtmp.normalize().mul(d2s);
        rtmp.mul(scaled);
        drawradarent(rtmp.x, rtmp.y, 0, b->owner == p ? 2 : isteam(b->owner, p) ? 1 : 0, 3, iconsize / 1.5f, 1);
    }
    glEnable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    loopv(radar_explosions) // explosions
    {
        int ndelay = lastmillis - radar_explosions[i].millis;
        if (ndelay > 600) radar_explosions.remove(i--);
        else
        {
            static GLubyte col_ownexp[4] = { 0xf7, 0xf5, 0x34 }; // yellow for your own explosions
            static GLubyte col_friendlyexp[4] = { 0x02, 0x13, 0xFB }; // blue for friendlies' explosions
            static GLubyte col_enemyexp[4] = { 0xFB, 0x02, 0x02 }; // red for enemies' explosions
            GLubyte *col;
            if (radar_explosions[i].owner == p) col = col_ownexp;
            else if (isteam(p, radar_explosions[i].owner)) col = col_friendlyexp;
            else col = col_enemyexp;
            vec nxpo(radar_explosions[i].o[0], radar_explosions[i].o[1], 0);
            nxpo.sub(p->o);
            if (nxpo.magnitude() > d2s)
                nxpo.normalize().mul(d2s);
            nxpo.mul(scaled);
            if (ndelay < 400)
            {
                col[3] = (1.f - ndelay / 400.f) * 255;
                DrawCircle(nxpo.x, nxpo.y, ndelay / 100.f * scaled, col, 2.f);
            }
            col[3] = (1.f - ndelay / 600.f) * 255;
            DrawCircle(nxpo.x, nxpo.y, pow(ndelay, 1.5f) / 3094.0923f * scaled, col);
        }
    }
    loopv(radar_shotlines) // shotlines
    {
        if (radar_shotlines[i].expire < lastmillis) radar_shotlines.remove(i--);
        else
        {
            static const GLubyte col_ownshot[3] = { 0x94, 0xB0, 0xDE }; // blue for your shots
            static const GLubyte col_friendlyshot[3] = { 0xB8, 0xDC, 0x78 }; // light green-yellow for friendlies
            static const GLubyte col_enemyshot[3] = { 0xFF, 0xFF, 0xFF }; // white for enemies
            const GLubyte *col;
            if (radar_shotlines[i].owner == p) col = col_ownshot;
            else if (isteam(p, radar_shotlines[i].owner)) col = col_friendlyshot;
            else col = col_enemyshot;
            glBegin(GL_LINES);
            vec from(radar_shotlines[i].from[0], radar_shotlines[i].from[1], 0), to(radar_shotlines[i].to[0], radar_shotlines[i].to[1], 0);
            from.sub(p->o);
            to.sub(p->o);
            if (from.magnitude() > d2s)
                from.normalize().mul(d2s);
            if (to.magnitude() > d2s)
                to.normalize().mul(d2s);
            // source shot
            glColor4ub(col[0], col[1], col[2], 200);
            glVertex2f(from.x*scaled, from.y*scaled);
            // dest shot
            glColor4ub(col[0], col[1], col[2], 250);
            glVertex2f(to.x*scaled, to.y*scaled);
            glEnd();
        }
    }
    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
    // eye candy:
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(1, 1, 1);
    static Texture *bordertex = NULL;
    if(!bordertex) bordertex = textureload("packages/misc/compass-base.png", 3);
    quad(bordertex->id, centerpos.x-halfviewsize-16, centerpos.y-halfviewsize-16, radarviewsize+32, 0, 0, 1, 1);
    if (show_hud_element(!hidecompass, 5))
    {
        static Texture *compasstex = NULL;
        if(!compasstex) compasstex = textureload("packages/misc/compass-rose.png", 3);
        glPushMatrix();
        glTranslatef(centerpos.x, centerpos.y, 0);
        glRotatef(-camera1->yaw, 0, 0, 1);
        quad(compasstex->id, -halfviewsize-8, -halfviewsize-8, radarviewsize+16, 0, 0, 1, 1);
        glPopMatrix();
    }

}

void drawradar(playerent *p, int w, int h)
{
    if(showmap) drawradar_showmap(p,w,h);
    else drawradar_vicinity(p,w,h);
}

void drawteamicons(int w, int h)
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(1, 1, 1);
    static Texture *icons = NULL;
    if(!icons) icons = textureload("packages/misc/teamicons.png", 3);
    quad(icons->id, VIRTW-VIRTH/12-10, 10, VIRTH/12, team_base(focus->team) ? 0.5f : 0, 0, 0.49f, 1.0f);
}

int damageblendmillis = 0;

VARFP(damagescreen, 0, 1, 1, { if(!damagescreen) damageblendmillis = 0; });
VARP(damagescreenfactor, 1, 4, 100); // only for non-regen modes
VARP(damagescreenalpha, 1, 55, 100);
VARP(damagescreenfade, 0, 325, 1000); // only for non-regen modes

void damageblend(int n)
{
    if(!damagescreen || m_regen(gamemode, mutators)) return;
    if (n < 0)
    {
        // clear damage screen
        damageblendmillis = 0;
    }
    else
    {
        // extend by up to 5 seconds
        if (lastmillis > damageblendmillis) damageblendmillis = lastmillis;
        damageblendmillis += min(n*damagescreenfactor, 5000);
    }
}

string enginestateinfo = "";
void CSgetEngineState() { result(enginestateinfo); }
COMMANDN(getEngineState, CSgetEngineState, "");

VARP(clockdisplay,0,0,2);
VARP(dbgpos,0,0,1);
VARP(showtargetname,0,1,1);
VARP(showspeed, 0, 0, 1);
VARP(monitors, 1, 1, 12);

static char lastseen [20];
void lasttarget() { result(lastseen); }
COMMAND(lasttarget, "");

inline int mm_adjust(int x)
{
    return ((monitors + (((x << 1) - 1) << ((monitors & 1) ^ 1))) * VIRTW / monitors) >> 1;
    /*
    // original
    if(monitors & 1) return (2*x + monitors - 1)*VIRTW/(2*monitors);
    else return (4*x + monitors - 2)*VIRTW/(2*monitors);
    */
}

int votersort(playerent **a, playerent **b)
{
    return (*a)->voternum - (*b)->voternum;
}

void gl_drawhud(int w, int h, int curfps, int nquads, int curvert, bool underwater)
{
    bool spectating = player1->isspectating();
    int origVIRTW = VIRTW;

    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, VIRTW, VIRTH, 0, -1, 1);
    glTranslatef((float)VIRTW*(monitors - 2 + (monitors&1))/(2.*monitors), 0., 0.);
    VIRTW /= (float)monitors/(float)(2 - (monitors & 1));
    glEnable(GL_BLEND);

    if(underwater)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4ub(hdr.watercolor[0], hdr.watercolor[1], hdr.watercolor[2], 102);

        glBegin(GL_TRIANGLE_STRIP);
        glVertex2f(0, 0);
        glVertex2f(VIRTW, 0);
        glVertex2f(0, VIRTH);
        glVertex2f(VIRTW, VIRTH);
        glEnd();
    }

    glEnable(GL_TEXTURE_2D);

    // damage screen
    if(damagescreen)
    {
        static float fade = 0;
        if (m_regen(gamemode, mutators))
        {
            const int maxhealth = 100 * HEALTHSCALE;
            float newfade = 0;
            if (focus->state == CS_ALIVE && focus->health >= 0 && focus->health < maxhealth)
                newfade = sqrtf(1.f - focus->health / (float)maxhealth);
            fade = clamp((fade * 40.f + newfade) / 41.f, 0.f, 1.f);
        }
        else if (lastmillis < damageblendmillis)
        {
            fade = 1.f;
            if (damageblendmillis - lastmillis < damagescreenfade)
                fade *= (damageblendmillis - lastmillis) / (float)damagescreenfade;
        }
        else fade = 0;

        if (fade >= 0.05f)
        {
            static Texture *damagetex = NULL;
            if (!damagetex) damagetex = textureload("packages/misc/damage.png", 3);

            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBindTexture(GL_TEXTURE_2D, damagetex->id);
            glColor4f(1, 1, 1, fade * damagescreenalpha / 100.f);

            glBegin(GL_TRIANGLE_STRIP);
            glTexCoord2f(0, 0); glVertex2f(0, 0);
            glTexCoord2f(1, 0); glVertex2f(VIRTW, 0);
            glTexCoord2f(0, 1); glVertex2f(0, VIRTH);
            glTexCoord2f(1, 1); glVertex2f(VIRTW, VIRTH);
            glEnd();
        }
    }
    // damage direction
    drawdmgindicator();

    if (worldhit) strcpy(lastseen, worldhit->name);
    bool menu = menuvisible();
    bool command = getcurcommand() ? true : false;
    bool reloading = lastmillis < focus->weaponsel->reloading + focus->weaponsel->info.reloadtime;
    if (focus->state != CS_DEAD && !reloading)
    {
        const int teamtype = worldhit && worldhit->state == CS_ALIVE ? isteam(worldhit, focus) ? 1 : 2 : 0;
        focus->weaponsel->renderaimhelp(teamtype);
    }

    drawhitmarker();

    // TODO: fake red dot

    // event icons
    static Texture **texs = geteventicons();
    if (!isthirdperson) loopv(focus->icons)
    {
        eventicon &icon = focus->icons[i];
        if (icon.type < 0 || icon.type >= eventicon::TOTAL || icon.millis + 3000 < lastmillis)
        {
            focus->icons.remove(i--);
            continue;
        }
        Texture *tex = texs[icon.type];
        int h = 1;
        float aspect = 1, scalef = 1, offset = (lastmillis - icon.millis) / 3000.f * 160.f;
        switch (icon.type)
        {
            case eventicon::CHAT:
            case eventicon::VOICECOM:
            case eventicon::PICKUP:
                scalef = .4f;
                break;
            case eventicon::HEADSHOT:
            case eventicon::CRITICAL:
            case eventicon::REVENGE:
            case eventicon::FIRSTBLOOD:
                aspect = 2;
                h = 4;
                break;
            case eventicon::DECAPITATED:
            case eventicon::BLEED:
                scalef = .4f;
                break;
            default:
                scalef = .3f;
                break;
        }
        glBindTexture(GL_TEXTURE_2D, tex->id);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glColor4f(1.f, 1.f, 1.f, (3000 + icon.millis - lastmillis) / 3000.f);
        glBegin(GL_QUADS);
        float anim = lastmillis / 100 % (h * 2);
        if (anim >= h) anim = h * 2 - anim + 1;
        anim /= h;
        const float xx = VIRTH * .15f * scalef, yy = /*VIRTH * .2f * scalef*/ xx / aspect, yoffset = VIRTH * -.15f - offset;
        glTexCoord2f(0, anim); glVertex2f(VIRTW / 2 - xx, VIRTH / 2 - yy + yoffset);
        glTexCoord2f(1, anim); glVertex2f(VIRTW / 2 + xx, VIRTH / 2 - yy + yoffset);
        anim += 1.f / h;
        glTexCoord2f(1, anim); glVertex2f(VIRTW / 2 + xx, VIRTH / 2 + yy + yoffset);
        glTexCoord2f(0, anim); glVertex2f(VIRTW / 2 - xx, VIRTH / 2 + yy + yoffset);
        glEnd();
    }

    if (focus->state == CS_ALIVE && show_hud_element(!hidehudequipment, 3)) drawequipicons(focus);

    if (/*!menu &&*/ (show_hud_element(!hideradar, 5) || showmap)) drawradar(focus, w, h);
    //if(showsgpat) drawsgpat(w,h); // shotty
    if(!editmode)
    {
        glMatrixMode(GL_MODELVIEW);
        if (show_hud_element(!hideteam, 1) && m_team(gamemode, mutators)) drawteamicons(w, h);
        glMatrixMode(GL_PROJECTION);
    }

    char *infostr = editinfo();
    int commandh = 1570 + FONTH;
    if(command) commandh -= rendercommand(20, 1570, VIRTW);
    else if(infostr) draw_text(infostr, 20, 1570);
    else if(show_hud_element(!hidehudtarget, 1))
    {
        defformatstring(hudtext)("\f0[\f1%04.1f\f3m\f0]", focus->o.dist(worldhitpos) / CUBES_PER_METER);
        static string hudtarget;
        static int lasttarget = INT_MIN;
        if(worldhit)
        {
            formatstring(hudtarget)(" \f2[\f%d%s\f2] \f4[\f%s\f4]", team_rel_color(focus, worldhit), colorname(worldhit),
                worldhitzone==HIT_HEAD?"3HEAD":worldhitzone==HIT_TORSO?"2TORSO":"0LEGS");
            concatstring(hudtext, hudtarget);
            lasttarget = lastmillis;
        }
        else if(lastmillis - lasttarget < 800)
        {
            const short a = (800 - lastmillis + lasttarget) * 255 / 800;
            draw_text(hudtarget, 20 + text_width(hudtext), 1570, a, a, a, a);
        }
        draw_text(hudtext, 20, 1570);
    }

    extern int lastexpadd, lastexptexttime;
    if (lastmillis <= lastexpadd + COMBOTIME)
    {
        extern int lastexpaddamt;
        defformatstring(scoreaddtxt)("\f%c%+d", !lastexpaddamt ? '4' : lastexpaddamt >= 0 ? '2' : '3', lastexpaddamt);
        const short a = (lastexpadd + COMBOTIME - lastmillis) * 255 / COMBOTIME;
        draw_text(scoreaddtxt, VIRTW * 11 / 20, VIRTH * 8 / 20, a, a, a, a);
    }

    if (lastmillis <= lastexptexttime + COMBOTIME)
    {
        extern string lastexptext;
        const short a = (lastexptexttime + COMBOTIME - lastmillis) * 255 / COMBOTIME;
        draw_text(lastexptext, VIRTW * 11 / 20, VIRTH * 8 / 20 + FONTH, a, a, a, a);
    }

    glLoadIdentity();
    glOrtho(0, origVIRTW*2, VIRTH*2, 0, -1, 1);
    glTranslatef((float)origVIRTW*(float)((float)monitors - 2. + (float)(monitors&1))/((float)monitors), 0., 0.);
    extern int tsens(int x);
    tsens(-2000);
    extern void r_accuracy(int h);
    if (!spectating) r_accuracy(commandh);
    if (hud_must_not_override(!hideconsole)) renderconsole();
    VIRTW=origVIRTW;
    if (show_hud_element(!hideobits, 6)) renderobits();
    VIRTW /= (float)monitors/(float)(2 - (monitors & 1));
    formatstring(enginestateinfo)("%d %d %d %d %d", curfps, lod_factor(), nquads, curvert, xtraverts);
    if(showstats)
    {
        if(showstats==2)
        {
            const int left = (VIRTW-225-10)*2, top = (VIRTH*7/8)*2;
            const int ttll = VIRTW*2 - 3*FONTH/2;
            blendbox(left - 24, top - 24, VIRTW*2 - 72, VIRTH*2 - 48, true, -1);
            int c_num;
            int c_r = 255;      int c_g = 255;      int c_b = 255;
            string c_val;
    #define TXTCOLRGB \
            switch(c_num) \
            { \
                case 0: c_r = 120; c_g = 240; c_b = 120; break; \
                case 1: c_r = 120; c_g = 120; c_b = 240; break; \
                case 2: c_r = 230; c_g = 230; c_b = 110; break; \
                case 3: c_r = 250; c_g = 100; c_b = 100; break; \
                default: \
                    c_r = 255; c_g = 255; c_b =  64; \
                break; \
            }

            draw_text("fps", left - (text_width("fps") + FONTH/2), top    );
            draw_text("lod", left - (text_width("lod") + FONTH/2), top+ 80);
            draw_text("wqd", left - (text_width("wqd") + FONTH/2), top+160);
            draw_text("wvt", left - (text_width("wvt") + FONTH/2), top+240);
            draw_text("evt", left - (text_width("evt") + FONTH/2), top+320);

            //ttll -= 3*FONTH/2;

            formatstring(c_val)("%d", curfps);
            c_num = curfps > 150 ? 0 : (curfps > 100 ? 1 : (curfps > 30 ? 2 : 3)); TXTCOLRGB
            draw_text(c_val, ttll - text_width(c_val), top    , c_r, c_g, c_b);

            int lf = lod_factor();
            formatstring(c_val)("%d", lf);
            c_num = lf>199?(lf>299?(lf>399?3:2):1):0; TXTCOLRGB
            draw_text(c_val, ttll - text_width(c_val), top+ 80, c_r, c_g, c_b);

            formatstring(c_val)("%d", nquads);
            c_num = nquads>3999?(nquads>5999?(nquads>7999?3:2):1):0; TXTCOLRGB
            draw_text(c_val, ttll - text_width(c_val), top+160, c_r, c_g, c_b);

            formatstring(c_val)("%d", curvert);
            c_num = curvert>3999?(curvert>5999?(curvert>7999?3:2):1):0; TXTCOLRGB
            draw_text(c_val, ttll - text_width(c_val), top+240, c_r, c_g, c_b);

            formatstring(c_val)("%d", xtraverts);
            c_num = xtraverts>3999?(xtraverts>5999?(xtraverts>7999?3:2):1):0; TXTCOLRGB
            draw_text(c_val, ttll - text_width(c_val), top+320, c_r, c_g, c_b);
        }
        else
        {
            if(dbgpos)
            {
                pushfont("mono");
                defformatstring(o_yw)("%05.2f YAW", player1->yaw);
                draw_text(o_yw, VIRTW*2 - ( text_width(o_yw) + FONTH ), VIRTH*2 - 15*FONTH/2);
                defformatstring(o_p)("%05.2f PIT", player1->pitch);
                draw_text(o_p, VIRTW*2 - ( text_width(o_p) + FONTH ), VIRTH*2 - 13*FONTH/2);
                defformatstring(o_x)("%05.2f X  ", player1->o.x);
                draw_text(o_x, VIRTW*2 - ( text_width(o_x) + FONTH ), VIRTH*2 - 11*FONTH/2);
                defformatstring(o_y)("%05.2f Y  ", player1->o.y);
                draw_text(o_y, VIRTW*2 - ( text_width(o_y) + FONTH ), VIRTH*2 - 9*FONTH/2);
                defformatstring(o_z)("%05.2f Z  ", player1->o.z);
                draw_text(o_z, VIRTW*2 - ( text_width(o_z) + FONTH ), VIRTH*2 - 7*FONTH/2);
                popfont();
            }
            defformatstring(c_val)("fps %d", curfps);
            draw_text(c_val, VIRTW*2 - ( text_width(c_val) + FONTH ), VIRTH*2 - 3*FONTH/2);
        }
    }
    if(!intermission && clockdisplay!=0 && lastgametimeupdate!=0)
    {
        string gtime;
        int cssec = (gametimecurrent+(lastmillis-lastgametimeupdate))/1000;
        int gtsec = cssec%60;
        int gtmin = cssec/60;
        if(clockdisplay==1)
        {
            int gtmax = gametimemaximum/60000;
            gtmin = gtmax - gtmin;
            if(gtsec!=0)
            {
                gtmin -= 1;
                gtsec = 60 - gtsec;
            }
        }
        formatstring(gtime)("%02d:%02d", gtmin, gtsec);
        draw_text(gtime, (2*VIRTW - text_width(gtime))/2, 2);
    }

    if (hud_must_not_override(hidevote < 2))
    {
        extern votedisplayinfo *curvote;

        if (curvote && curvote->millis >= totalmillis && !(hud_must_not_override(hidevote == 1) && player1->vote != VOTE_NEUTRAL && curvote->result == VOTE_NEUTRAL))
        {
            int left = 20*2, top = VIRTH;
            if (curvote->result == VOTE_NEUTRAL)
                draw_textf("%s called a vote: %.2f seconds remaining", left, top + 240, curvote->owner ? colorname(curvote->owner) : "(unknown)", (curvote->expiremillis - lastmillis) / 1000.0f);
            else
                draw_textf("%s called a vote:", left, top+240, curvote->owner ? colorname(curvote->owner) : "(unknown)");
            draw_textf("%s", left, top+320, curvote->desc);
            draw_textf("----", left, top+400);

            vector<playerent *> votepl[VOTE_NUM];
            string votestr[VOTE_NUM];
            if (!watchingdemo) votepl[player1->vote].add(player1);
            loopv(players)
            {
                playerent *vpl = players[i];
                if (!vpl || vpl->ownernum >= 0) continue;
                votepl[vpl->vote].add(vpl);
            }
            loopl(VOTE_NUM)
            {
                copystring(votestr[l], "");
                if (!votepl[l].length()) continue;
                // special case: hide if too many are neutral
                if (l == VOTE_NEUTRAL && votepl[VOTE_NEUTRAL].length() > 5) continue;
                votepl[l].sort(votersort);
                loopv(votepl[l])
                {
                    playerent *vpl = votepl[l][i];
                    if (!vpl) continue;
                    concatformatstring(votestr[l], "\f%d%s \f6(%d)", vpl->clientrole ? 0 : vpl == player1 ? 6 : team_color(vpl->team), vpl->name, vpl->clientnum);
                    if (vpl->clientrole >= CR_ADMIN) concatstring(votestr[l], " \f8(!)");
                    concatstring(votestr[l], "\f5, ");
                }
                // trim off last space, comma, 5, and line feed
                votestr[l][strlen(votestr[l]) - 4] = '\0';
                //copystring(votestr[l], votestr[l], strlen(votestr[l])-1);
            }
            draw_textf("\fs\f%c%d yes\fr vs. \fs\f%c%d no\fr", left, top + 480,
                curvote->expiryresult == VOTE_YES ? '0' : '5',
                votepl[VOTE_YES].length(),
                curvote->expiryresult == VOTE_NO ? '3' : '5',
                votepl[VOTE_NO].length());

            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glColor4f(1.0f, 1.0f, 1.0f, (sinf(lastmillis/100.0f)+1.0f) / 2.0f);
            switch(curvote->result)
            {
                case VOTE_NEUTRAL:
                    drawvoteicon(left, top, 0, 0, true);
                    if (player1->vote == VOTE_NEUTRAL)
                        draw_textf("\f3please vote yes or no (F1/F2)", left, top+560);
                    else
                        draw_textf("\f2you voted \f%s \f1(F%d to change)", left, top + 560, player1->vote == VOTE_NO ? "3no" : "0yes", player1->vote == VOTE_NO ? 1 : 2);
                    break;
                default:
                    drawvoteicon(left, top, (curvote->result-1)&1, 1, false);
                    draw_textf("\f%s \f%s", left, top+560, curvote->veto ? "1VETO" : "2vote", curvote->result == VOTE_YES ? "0PASSED" : "3FAILED");
                    break;
            }
            glLoadIdentity();
            glOrtho(0, VIRTW*2.2, VIRTH*2.2, 0, -1, 1);
            left *= 1.1; top += 560; top *= 1.1;
            if (*votestr[VOTE_YES])
            {
                draw_textf("\f1Vote \f0Yes \f5(\f4%d/%d\f5)", left, top += 88, votepl[VOTE_YES].length(), curvote->yes_remain);
                draw_text(votestr[VOTE_YES], left, top += 88);
            }
            if (*votestr[VOTE_NO])
            {
                draw_textf("\f1Vote \f3No \f5(\f4%d/%d\f5)", left, top += 88, votepl[VOTE_NO].length(), curvote->no_remain);
                draw_text(votestr[VOTE_NO], left, top += 88);
            }
            if (*votestr[VOTE_NEUTRAL])
            {
                draw_textf("\f1Vote \f2Neutral \f5(\f4%d\f5)", left, top += 88, votepl[VOTE_NEUTRAL].length());
                draw_text(votestr[VOTE_NEUTRAL], left, top += 88);
            }
        }
    }
    //else draw_textf("%c%d here F1/F2 will be praised during a vote", 20*2, VIRTH+560, '\f', 0); // see position (left/top) setting in block above

    if(menu) rendermenu();
    else if(command) renderdoc(40, VIRTH, max(commandh*2 - VIRTH, 0));

    VIRTW = origVIRTW;
    if (hud_must_not_override(!hidehudmsgs)) hudmsgs.render();
    VIRTW /= (float)monitors/(float)(2 - (monitors & 1));


    if (!hidespecthud && !menu && focus->state == CS_DEAD && focus->spectatemode <= SM_DEATHCAM)
    {
        glLoadIdentity();
        glOrtho(0, origVIRTW*3/2, VIRTH*3/2, 0, -1, 1);
        glTranslatef((float)origVIRTW*3*(monitors - 2 + (monitors&1))/(4.*monitors), 0., 0.);
        const int left = (VIRTW*3/2)*6/8, top = (VIRTH*3/2)*3/4;
        draw_textf("SPACE to change view", left, top);
        draw_textf("SCROLL to change player", left, top+80);
    }

    /* * /
    glLoadIdentity();
    glOrtho(0, VIRTW*3/2, VIRTH*3/2, 0, -1, 1);
    const int tbMSGleft = (VIRTW*3/2)*5/6;
    const int tbMSGtop = (VIRTH*3/2)*7/8;
    draw_textf("!TEST BUILD!", tbMSGleft, tbMSGtop);
    / * */

    if(showspeed)
    {
        glLoadIdentity();
        glPushMatrix();
        glOrtho(0, origVIRTW, VIRTH, 0, -1, 1);
        glTranslatef((float)origVIRTW*(monitors - 2 + (monitors&1))/(2.*monitors), 0., 0.);
        glScalef(0.8, 0.8, 1);
        draw_textf("Speed: %.2f", VIRTW / 2, VIRTH, focus->vel.magnitudexy());
        glPopMatrix();
    }

    if(!hidespecthud && spectating && player1->spectatemode!=SM_DEATHCAM)
    {
        glLoadIdentity();
        glOrtho(0, origVIRTW, VIRTH, 0, -1, 1);
        glTranslatef((float)origVIRTW*(monitors - 2 + (monitors&1))/(2.*monitors), 0., 0.);
        const char *specttext = "GHOST";
        if(player1->team == TEAM_SPECT) specttext = "GHOST";
        else if(player1->team == TEAM_CLA_SPECT) specttext = "[CLA]";
        else if(player1->team == TEAM_RVSF_SPECT) specttext = "[RVSF]";
        draw_text(specttext, VIRTW/40, VIRTH/10*7);
        if(focus != player1)
        {
            defformatstring(name)("Player %s", focus->name);
            draw_text(name, VIRTW/40, VIRTH/10*8);
        }
    }

    glLoadIdentity();
    glOrtho(0, origVIRTW/2, VIRTH/2, 0, -1, 1);
    glTranslatef((float)origVIRTW*(monitors - 2 + (monitors&1))/(4.*monitors), 0., 0.);

    if (show_hud_element(!hidehudequipment, 3) && focus->state != CS_DEAD && focus->state != CS_EDITING)
    {
        pushfont("huddigits");
        if (show_hud_element(!hidehudequipment, 1))
        {
            defformatstring(healthstr)("%d", focus->health / HEALTHSCALE);
            draw_text(healthstr, 90, 823);
            if (focus->armour)
            {
                int offset = text_width(healthstr);
                glPushMatrix();
                glScalef(0.5f, 0.5f, 1.0f);
                draw_textf("%d", (90 + offset) * 2, 823 * 2, focus->armour);
                glPopMatrix();
            }
        }
        if (focus->weaponsel && focus->weaponsel->type >= GUN_KNIFE && focus->weaponsel->type<NUMGUNS)
        {
            glMatrixMode(GL_MODELVIEW);
            if (focus->weaponsel->type != GUN_GRENADE) focus->weaponsel->renderstats();
            else if (focus->prevweaponsel && focus->prevweaponsel->type != GUN_GRENADE) focus->prevweaponsel->renderstats();
            else if (focus->nextweaponsel && focus->nextweaponsel->type != GUN_GRENADE) focus->nextweaponsel->renderstats();
            // if(p->mag[GUN_GRENADE]) p->weapons[GUN_GRENADE]->renderstats();
            glMatrixMode(GL_PROJECTION);
        }
        popfont();
    }

    if(m_flags(gamemode) && !hidectfhud)
    {
        glLoadIdentity();
        glOrtho(0, origVIRTW, VIRTH, 0, -1, 1);
        glTranslatef((float)origVIRTW*(monitors - 2 + (monitors&1))/(2.*monitors), 0., 0.);
        glColor4f(1.0f, 1.0f, 1.0f, 0.2f);
        turn_on_transparency(255);

        loopi(2) // flag state
        {
            drawflagicons(flaginfos[i], focus);
            if(m_team(gamemode, mutators))
            {
                defformatstring(count)("%d", teamscores[i].flagscore);
                int cw, ch;
                text_bounds(count, cw, ch);
                draw_textf(count, i*120+VIRTW/4.0f*3.0f+60-cw/2, 1590);
            }
        }
    }

    // perk icons
    glLoadIdentity();
    glOrtho(0, origVIRTW, VIRTH, 0, -1, 1);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTranslatef((float)VIRTW*(monitors - 2 + (monitors&1))/(2.*monitors), 0., 0.);

    if (show_hud_element(!hidehudequipment, 6))
    {
        static Texture *perktex1[PERK1_MAX] = { NULL }, *perktex2[PERK1_MAX] = { NULL };
        if (!perktex1[0])
        {
            const char *perktexname1[PERK1_MAX] = { "none", "radar", "ninja", "power", "time", "speed", "hand", "light", "point" };
            const char *perktexname2[PERK2_MAX] = { "none", "radar", "ninja", "power", "time", "vision", "streak", "steady", "health", };
            loopi(PERK1_MAX)
            {
                if (perktex1[i]) continue;
                defformatstring(tname)("packages/perks/%s.png", perktexname1[i]);
                perktex1[i] = textureload(tname);
            }
            loopi(PERK2_MAX)
            {
                if (perktex2[i]) continue;
                defformatstring(tname)("packages/perks/%s.png", perktexname2[i]);
                perktex2[i] = textureload(tname);
            }
        }
        Texture *perk1 = perktex1[focus->perk1%PERK1_MAX], *perk2 = perktex2[focus->perk2%PERK2_MAX];
        if (perk1 != perk2)
        {
            glColor4f(1.0f, 1.0f, 1.0f, focus->perk1 /* != PERK_NONE */ && focus->state != CS_DEAD ? .78f : .3f);
            quad(perk1->id, VIRTW - 440 - 15 - 100, VIRTH - 100 - 10, 100, 0, 0, 1);
        }

        if (perk2)
        {
            glColor4f(1.0f, 1.0f, 1.0f, focus->perk2 /* != PERK_NONE */ && focus->state != CS_DEAD ? .78f : .3f);
            quad(perk2->id, VIRTW - 440, VIRTH - 100 - 10, 100, 0, 0, 1);
        }
    }

    // streak meter
    if (show_hud_element(!hidehudequipment, 1))
    {
        const float streakscale = 1.5f;
        static Texture *streakt[2][4] = { { NULL } }; // TODO: use native plain OpenGL triangles?
        loopi(2) loopj(4)
        {
            // done, current, outstanding
            defformatstring(path)("packages/streak/%d%s.png", i, j ? j > 1 ? j > 2 ? "d" : "" : "c" : "o");
            streakt[i][j] = textureload(path);
        }
        glLoadIdentity();
        glOrtho(0, origVIRTW * streakscale, VIRTH * streakscale, 0, -1, 1);
        glTranslatef((float)streakscale*origVIRTW*(monitors - 2 + (monitors&1))/(2.*monitors), 0., 0.);
        // we have the blend function set by the perk icon
        const int currentstreak = floor(focus->pointstreak / 5.f);
        loopi(11){
            glColor4f(1, 1, 1, focus->state != CS_DEAD ? (currentstreak == i || i >= 10) ? (0.3f + fabs(sinf(lastmillis / 500.0f)) / 2 * ((i - 1) % 5) / 4.f) : .8f : .3f);
            quad(streakt[i & 1][currentstreak > i ? 2 : currentstreak == i ? 1 : focus->deathstreak >= i ? 3 : 0]->id,
                (VIRTW - 620 - 15 - (11 * 50) + i * 50) * streakscale, (VIRTH - 80 - 35) * streakscale, 80 * streakscale, 0, 0, 1);
        }
        // streak misc
        // streak num
        if (focus->deathstreak) draw_textf("\f3-%d", (VIRTW - 620 - 12 - max(11 - focus->deathstreak, 1) * 50) * streakscale, (VIRTH - 50 - 40) * streakscale, focus->deathstreak);
        else draw_textf("\f%c%.1f", (VIRTW - 620 - 15 - max(11 - currentstreak, 1) * 50) * streakscale, (VIRTH - 50 - 40) * streakscale,
            focus->pointstreak >= 9 * 5 ? '1' :
            focus->pointstreak >= 7 * 5 ? '0' :
            focus->pointstreak >= 3 * 5 ? '2' :
            focus->pointstreak ? '2' :
            '4',
            focus->pointstreak / 5.f);
        // airstrikes
        draw_textf("\f4x\f%c%d", (VIRTW - 620 - 10 - 5 * 50) * streakscale, (VIRTH - 50) * streakscale, focus->airstrikes ? '0' : '5', focus->airstrikes);
        // radar time
        int stotal, sr;
        playerent *spl;
        radarinfo(stotal, spl, sr, focus);
        if (!sr || !spl) stotal = 0; // safety
        draw_textf("%d:\f%d%04.1f", (VIRTW - 620 - 40 - 3 * 50) * streakscale, (VIRTH - 50 - 80 - 25) * streakscale, stotal, stotal ? team_rel_color(focus, spl) : 5, sr / 1000.f);
        // nuke timer
        nukeinfo(stotal, spl, sr);
        if (!sr || !spl) stotal = 0; // more safety
        draw_textf("%d:\f%d%04.1f", (VIRTW - 620 - 40 - 50) * streakscale, (VIRTH - 50) * streakscale, stotal, stotal ? team_rel_color(focus, spl) : 5, sr / 1000.f);
    }

    VIRTW = origVIRTW;

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW);
}

void loadingscreen(const char *fmt, ...)
{
    static Texture *logo = NULL;
    if(!logo) logo = textureload("packages/misc/startscreen.png", 3);

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, VIRTW, VIRTH, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0, 0, 0, 1);
    glColor3f(1, 1, 1);

    loopi(fmt ? 1 : 2)
    {
        glClear(GL_COLOR_BUFFER_BIT);
        quad(logo->id, (VIRTW-VIRTH)/2, 0, VIRTH, 0, 0, 1);
        if(fmt)
        {
            glEnable(GL_BLEND);
            defvformatstring(str, fmt, fmt);
            int w = text_width(str);
            draw_text(str, w>=VIRTW ? 0 : (VIRTW-w)/2, VIRTH*3/4);
            glDisable(GL_BLEND);
        }
        SDL_GL_SwapBuffers();
    }

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
}

static void bar(float bar, int o, float r, float g, float b)
{
    int side = 2*FONTH;
    float x1 = side, x2 = bar*(VIRTW*1.2f-2*side)+side;
    float y1 = o*FONTH;
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_TRIANGLE_STRIP);
    loopk(10)
    {
       float c = 1.2f*cosf(M_PI/2 + k/9.0f*M_PI), s = 1 + 1.2f*sinf(M_PI/2 + k/9.0f*M_PI);
       glVertex2f(x2 - c*FONTH, y1 + s*FONTH);
       glVertex2f(x1 + c*FONTH, y1 + s*FONTH);
    }
    glEnd();

    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_STRIP);
    loopk(10)
    {
       float c = cosf(M_PI/2 + k/9.0f*M_PI), s = 1 + sinf(M_PI/2 + k/9.0f*M_PI);
       glVertex2f(x2 - c*FONTH, y1 + s*FONTH);
       glVertex2f(x1 + c*FONTH, y1 + s*FONTH);
    }
    glEnd();
}

void show_out_of_renderloop_progress(float bar1, const char *text1, float bar2, const char *text2)   // also used during loading
{
    c2skeepalive();

    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, VIRTW*1.2f, VIRTH*1.2f, 0, -1, 1);

    glLineWidth(3);

    if(text1)
    {
        bar(1, 1, 0.1f, 0.1f, 0.1f);
        if(bar1>0) bar(bar1, 1, 0.2f, 0.2f, 0.2f);
    }

    if(bar2>0)
    {
        bar(1, 3, 0.1f, 0.1f, 0.1f);
        bar(bar2, 3, 0.2f, 0.2f, 0.2f);
    }

    glLineWidth(1);

    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    if(text1) draw_text(text1, 2*FONTH, 1*FONTH + FONTH/2);
    if(bar2>0) draw_text(text2, 2*FONTH, 3*FONTH + FONTH/2);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    SDL_GL_SwapBuffers();
}

