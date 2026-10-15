# iPhone Setup Guide

**English** | [Türkçe](iphone-kurulum.md)

Menu names can differ slightly between iOS versions. This guide was written for iOS 17–26.

## 0. Preparation

- **Create a second Apple Account.** It will only be used to recover the Screen Time passcode.
  - Give it a long password you won't remember, write it on paper, seal it in an envelope and keep it somewhere inconvenient.
  - Make sure you can access its recovery e-mail, otherwise recovery will not work later.
- Install the program on your PC (see the README), but **don't run it yet**.

## 1. Lock Screen Time with a temporary passcode

Settings → Screen Time → **Lock Screen Time Settings**

- Enter a **temporary passcode you know**, e.g. 1357. You will type it many times during the next steps.
- When asked for an Apple Account for recovery, enter the **second account**, not your main one.

## 2. Choose your mode

There are two modes. Don't combine them; pick one.

| | A) Hard mode | B) Balanced mode |
|---|---|---|
| Tool | Downtime | App Limits |
| Outside allowed hours | Almost everything is off; **every website in Safari is blocked, Google included** | Only the apps you chose are limited; Safari, maps, work apps stay usable |
| Social media | Only in your time window (e.g. 20:00–21:00) | A daily budget (e.g. 1 hour), whenever you want |
| For whom | People who want to cut phone access as hard as possible | People who need the phone during the day for work or school |

In both modes the browser block for social media sites (step 4) is active **all day**.

### A) Hard mode: Downtime

Screen Time → **Downtime**

1. Turn on **Scheduled** and set the hours. For example, off every day from 21:00 to 20:00 leaves the phone usable for one hour a day.
2. Turn on **Block at Downtime**.
3. Open Screen Time → **Always Allowed**. Remove any social media apps from that list. Phone, Messages, Maps and other essentials can stay.

> **Good to know:** during downtime iOS blocks **websites** too, not just apps. Safari opens if it is Always Allowed, but every site, google.com included, shows "You've reached your limit". There is no clean way to allow specific sites during downtime. If you need the internet during the day, choose mode B.

### B) Balanced mode: App Limits

Screen Time → **App Limits** → **Add Limit**

1. Select the **Social** category. To include apps from other categories (e.g. YouTube), expand that category and pick them one by one.
2. **Next** → set the time (e.g. **1 hour**). Use **Customize Days** for a different weekend limit.
3. **Block at End of Limit** must be **on**. If it is off, you can just tap "Ignore" when time runs out.
4. **Add**.
5. If the list contains a limit called **"All Apps & Categories"**, delete it; it limits Safari too.
6. Make sure Downtime's **Scheduled** option is off.

> **Good to know:** when the limit is reached, iOS offers "One More Minute" once without a passcode. Anything beyond that needs the Screen Time passcode. You can open the App Limits screen and **look** without a passcode, but **changing or deleting** a limit asks for it.

## 3. Content & Privacy Restrictions

Screen Time → **Content & Privacy Restrictions** → turn on the switch at the top.

**Installing and deleting apps.** Under **iTunes & App Store Purchases** (or **App Store Purchases**):

| Setting | Value |
|---|---|
| Installing Apps | Don't Allow |
| Deleting Apps | Don't Allow |
| In-app Purchases | Don't Allow (optional) |

This way you can't reset a block by deleting and reinstalling an app.

**Allow changes.** Further down on the same screen:

| Setting | Value |
|---|---|
| Passcode Changes | Don't Allow |
| Account Changes | Don't Allow |

**Against the clock trick.** Downtime follows the phone's clock, and changing the clock by hand can break the block:

1. Settings → General → Date & Time → **Set Automatically** must be on.
2. Settings → Privacy & Security → Location Services → System Services → **Setting Time Zone** must be on.
3. Content & Privacy Restrictions → **Location Services** → **Don't Allow**. This locks the settings above.

## 4. Browser block

Content Restrictions → **Web Content** → **Limit Adult Websites** → under **Never Allow**, tap **Add Website** and add:

```
instagram.com
tiktok.com
x.com
twitter.com
facebook.com
youtube.com
reddit.com
```

- This block is independent of Downtime or App Limits; it applies **all day**.
- If other browsers such as Chrome are installed, delete them.

## 5. Test

While you still have the temporary passcode, try to:

- **Mode A:** open a social media app during downtime. **Mode B:** temporarily set the limit to 1 minute and check that the app locks when it runs out.
- delete an app,
- open instagram.com in Safari,
- **Mode B:** search something on Google in Safari (this **must work**).

Everything that should be blocked must be blocked. If not, fix it with the temporary passcode.

## 6. Switch to the real passcode

1. On the PC, run the program → **2) First setup**.
2. Choose your confirmation sentence and type it twice.
3. On the iPhone: Settings → Screen Time → **Change Screen Time Passcode**:
   - As the old passcode, type your **temporary code** yourself.
   - The program shows the new code **digit by digit**. You enter it twice.
4. The program asks whether the phone accepted the new code; answer **Y**.
5. Back up `vault.dat` to a USB stick.

From now on, you don't know the passcode on your phone.

## Crisis plan

| Situation | What to do |
|---|---|
| `vault.dat` lost, backup exists | Copy the backup into the program's folder. |
| `vault.dat` lost, no backup | Change Screen Time Passcode → *Forgot Passcode?* → second Apple Account → new code → run First setup in the program. |
| Forgot the second account's password | iforgot.apple.com from another device. |
| Nothing works | Last resort: Settings → General → Transfer or Reset → Erase All Content and Settings (needs your main Apple Account password; back up first). |
| Emergency | Phone calls and emergency calls are never blocked. |

Don't try to guess the passcode on the phone: every wrong attempt increases the waiting time.
