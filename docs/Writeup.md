# Artefact report and commentary

My artefact is a set of computer graphics and physics demonstrations. Written
in a variety of languages, namely C++, OpenGL and JavaScript, these have been
great fun to research and code as well as providing an opportunity to exercise
my previously non-existant skills in project management.

### Idea and inspiration

I have done some programming before, but mainly in the context of problem
solving and algorithms. However, I have always been interested in the graphical
side.

I have followed the demoscene for a while. The demoscene 

*(See Appendix: Initial research/inspiration)*

### Planning and organisation

I had anticipated that learning OpenGL would be a massive endeavour. This is
why I decided to start my project as soon as possible, planning small
sub-projects to help me to get to grips with it. 

Right from the beginning before I had even started writing any code, I
carefully planned my program, structuring it into three parts. The most
important of the parts, the “library,” would be a set of useful and reusable
code, such as mesh management, texture and lighting code, that I could use in
each of my sub-projects without having redundant copy-and-pasted code
everywhere.

I knew from past experience that time management would be an issue for me
when doing the project. I worked very hard to combat this from the beginning of
the project; by setting myself targets and deadlines on my Timeline I could
keep track of .

*(See Appendix: Timeline plan)*

I used GitHub, an online

I put deadlines on the calendar on my phone. It would remind me a week or a day
before the deadline, so that I could prioritise things as they came. Often I
would find that  I had to juggle two or three time-consuming things on top of
my EPQ, such as UCAS applications or physics coursework, and it was at these
times that my calendar app came to my rescue, as I could efficiently see which
deadline was coming up first and prioritise accordingly.

In addition to this I tried something novel, which turned out to be
surprisingly effective. I would promise my friends that I would get a certain
part of my project done by a certain date, at which time I would show them my
progress so far. Closer to the date my friends would remind me to get on with
the project. Psychologically I found this extremely effective as I felt a very
strong pressure to not disappoint my friends when they had high hopes of me.

### Research and development

My research encompassed a large range of sources. As my artefact
is mostly computer-based, most of the relevant information that I have found
has been on the internet. However, this web-based information has taken the
forms of blog posts, research papers. Videos of presentations given at
conferences have also come in very useful.

My sources have mostly been factual or outlined procedures on how to achieve
things using different programming techniques. They have mostly been
independent of each other, however where there has been overlap they have been
in agreement, and often they have even cross-referenced each other. For
example, the Valve paper on text rendering was cross-referenced from Stack
Overflow, a website, and several other papers on OpenGL textures.

I think over the course of the EPQ I have greatly improved my abilities in
not only finding sources, but also using them effectively. At the beginning of
the project, I struggled to implement mesh rendering because I did not know how
to effectively turn the theory into code. In addition, I made two errors: I did
not write good test code that exercised mesh rendering, and I did not realise
that meshes were not the easiest solution to the problem that I was having, and
that there were better approaches.

*(See Appendix: Research diary and notetaking)*

### Evaluation and reflection

The physics engine worked fantastically. This is because I started work on the
physics engine relatively early, in April, and continued researching and
working on the engine over the course of four months. In addition an important
factor contributing to the success of the project was the fact that I set
myself checkpoints after implementing certain features of the physics engine,
which I would test by writing new JavaScript experiments around. For example
the ribbon example was written after I had implemented length constraints; and
the ragdoll example was written after collision detection was coded into the
engine.

The final demonstration, the bridge simulation, was intended to be slightly
larger in scope. Based on the experience and research I had done for previous
experiments, I was able to use Verlet integration and length and angle
constraints, which worked because of the thorough research I had done
beforehand. However, I could not write the collision detection in time. I had
expected to be able to use the code I had written to detect collisions in my
ragdoll game. This was not the case as my algorithm detected collisions between
a line and a point, whereas in my bridge simulation I had to find collisions
between two lines. I did not have time at the end to research and implement
this.

What was learned?

I learnt that it is important to start the project early. Leaving things to the
last minute doesn't work in a project as big as this one. In addition, I learnt
to set small quantifiable targets and checkpoints and testing those targets by
creating experiments in order to test that these work. I did not do this in the
first stages of my project, and this led to difficulties.

If I were to do my project again, or a similar project, I would definitely
carry on setting myself quantifiable targets. In addition to being a great way
of measuring progress, they are very good at motivating and guiding future
work. When I set achievable targets that I could see the results of. stopping
me from going off track and encouraging me to continue.



<!-- vim: set tw=79: -->
