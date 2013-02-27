  She walked into the room, small clouds of silvery dust swirling
about her ankles.

  It was as if time had chosen that exact moment to blow a gasket.
Every head was turned and every gaze frozen in a state of mixed
disbelief and awe.  The chatter soon died off entirely, and as she
smiled, a muted silence fell over the place.

  "I'm here to tell you about the revision to the ILLGOL programming
language," she began.

  A disappointed groan rumbled through the crowd.  Undaunted, the
young woman continued.

  "Illgola-2 adds several new features to the original ILLGOL.  It
should be backwards compatible, since none of the important semantics
were intentionally changed, but honestly, the original ILLGOL sources
haven't really been tested with it yet, so I can't say.

  "Anyway, among the features that were added for version two are the
following.

  "Some new commands for imperative contol flow operations were added:
go to, go back, and go check, specifically.

  "Some pseudo-non-imperative control flow operations were added:
should, hedge, cut, and paste.

  She paused and smiled.  "Am I going too fast for anybody?  No?  Okay,
moving on..."

  "There's now a bare minimum in terms of strong typing and local
variables with three special types of assignment, called 'TYPE',
'STRONG', and 'FORGET'.

  "You'll like this part.  Illgola-2 has a preprocessor, which handles
'INCLUDE' and 'DEFINE' directives.  This goes a long way to making
ILLGOL-2 a valid language for doing some serious software engineering
work in..."

  "This is starting to get a little weird," mumbled a voice in the
crowd, barely audible over the general murmur of dissent.

  "Yeah," spoke up another voice, this one a little firmer.  "I hadn't
expected release notes to come in the form of a short story involving
an unnamed young woman.  In fact, I don't think any of us did."  A
general sort of sound of mumbling arose from the group now in
agreement.

  "No?" said the beautiful young woman, who once again smiled in her
own uniquely intoxicating way.  "What if I pointed out that this wasn't
really a short story, but only a dialogue?  In fact, I'm doing most of
the talking, so it's more like a monologue."  That shut them up.

  "Carrying on...  the preprocessor is seperate from the compiler; it
was just 'loads easier' that way, apparently.  It also happens to be
written in Perl.  The same reason (being 'loads easier') is given.
I hear that it does full macro substitution complete with arguments
and even recursion.

  "Furthermore, we have some new file access commands, in fact there
are a lot of them.  Disk files, no less!  Let's see, there's in file,
out file, open, close, create, and eof and seek (both a query and an
update operation for those last two.)

  "Lastly, the following device I/O commands: in dac, out dac,
poll tty, flush tty, and some funny thing involving the 'at' symbol
and the 'print' statement.

  "Well, anyway, they're all tested in the file 'prj\i2stuff.ill',
so you can see that example code there for yourself, and you can
figure out how to use these things and what they're supposed to do.

  "A word of warning, though.  The 'out dac' instructions seems to
have some problems.  As does 'go to' when you specify a line number,
which will quite frankly probably never work like one would expect.

  "Now, was anyone here looking for support for Full Moon Fever?"

  Nothing happened.  No hands were raised, no sudden shouts of "Oh,
yes, that'd be me, miss."  This state persisted for roughly five
seconds and was broken by an annoyed-sounding:

  "Just what the hell IS 'Full Moon Fever' anyway?"

  "Well," blushed the young woman, "as I understand it, it was the
author's very first language.  Just a simple scripting language for
orchestrating ANSI animations - you know, ASCII symbols dancing around
a screen by terminal control codes.  It wasn't even Turing-Complete.
Although it did have a limited form of looping, it couldn't calculate
worth a damn.

  "FMF 'compatibility' has always been a feature of ILLGOL, and it
remains in ILLGOL-2, although there are no plans to enhance it per se.

  "The author says he gave up on DEL and INS in the FMF 'compatibility
mode' in ILLGOL-2 and decided instead to simply rewrite the only two
FMF scripts that were ever written."

  "Let me guess," interrupted one of the faceless voices from the audience.
"It was just loads easier that way?"

  "Yup."

