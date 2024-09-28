#include "tom/commands/inspirecommand.hpp"

#include <array>
#include <random>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom::Commands
{

/* public */

InspireCommand::InspireCommand(Application &application, QCommandLineParser &parser)
    : Command(application, parser)
{}

int InspireCommand::run()
{
    Command::run();

    static const std::array inspires {
        u"Act only according to that maxim whereby you can, at the same time, will that it should become a universal law. - Immanuel Kant"_s,
        u"An unexamined life is not worth living. - Socrates"_s,
        u"Be present above all else. - Naval Ravikant"_s,
        u"Do what you can, with what you have, where you are. - Theodore Roosevelt"_s,
        u"Happiness is not something readymade. It comes from your own actions. - Dalai Lama"_s,
        u"He who is contented is rich. - Laozi"_s,
        u"I begin to speak only when I am certain what I will say is not better left unsaid. - Cato the Younger"_s,
        u"I have not failed. I've just found 10,000 ways that won't work. - Thomas Edison"_s,
        u"If you do not have a consistent goal in life, you can not live it in a consistent way. - Marcus Aurelius"_s,
        u"It is never too late to be what you might have been. - George Eliot"_s,
        u"It is not the man who has too little, but the man who craves more, that is poor. - Seneca"_s,
        u"It is quality rather than quantity that matters. - Lucius Annaeus Seneca"_s,
        u"Knowing is not enough; we must apply. Being willing is not enough; we must do. - Leonardo da Vinci"_s,
        u"Let all your things have their places; let each part of your business have its time. - Benjamin Franklin"_s,
        u"Live as if you were to die tomorrow. Learn as if you were to live forever. - Mahatma Gandhi"_s,
        u"No surplus words or unnecessary actions. - Marcus Aurelius"_s,
        u"Nothing worth having comes easy. - Theodore Roosevelt"_s,
        u"Order your soul. Reduce your wants. - Augustine"_s,
        u"People find pleasure in different ways. I find it in keeping my mind clear. - Marcus Aurelius"_s,
        u"Simplicity is an acquired taste. - Katharine Gerould"_s,
        u"Simplicity is the consequence of refined emotions. - Jean D'Alembert"_s,
        u"Simplicity is the essence of happiness. - Cedric Bledsoe"_s,
        u"Simplicity is the ultimate sophistication. - Leonardo da Vinci"_s,
        u"Smile, breathe, and go slowly. - Thich Nhat Hanh"_s,
        u"The only way to do great work is to love what you do. - Steve Jobs"_s,
        u"The whole future lies in uncertainty: live immediately. - Seneca"_s,
        u"Very little is needed to make a happy life. - Marcus Aurelius"_s,
        u"Waste no more time arguing what a good man should be, be one. - Marcus Aurelius"_s,
        u"Well begun is half done. - Aristotle"_s,
        u"When there is no desire, all things are at peace. - Laozi"_s,
        u"Walk as if you are kissing the Earth with your feet. - Thich Nhat Hanh"_s,
        u"Because you are alive, everything is possible. - Thich Nhat Hanh"_s,
        u"Breathing in, I calm body and mind. Breathing out, I smile. - Thich Nhat Hanh"_s,
        u"Life is available only in the present moment. - Thich Nhat Hanh"_s,
        u"The best way to take care of the future is to take care of the present moment. - Thich Nhat Hanh"_s,
        u"Nothing in life is to be feared, it is only to be understood. Now is the time to understand more, so that we may fear less. - Marie Curie"_s,
    };

    static const auto size = inspires.size();

    // Obtain a random number from hardware
    std::random_device rd;
    // Seed the generator
    std::default_random_engine generator(rd());
    // Define the range
    std::uniform_int_distribution<typename decltype (inspires)::size_type> // NOLINT(misc-const-correctness)
    distribute(0, size - 1); // -1 is ok because it's 0-based

    comment(inspires.at(distribute(generator)));

    // Alternative implementation
//    std::srand(std::time(nullptr));
//    comment(inspires.at(std::rand() % size));

    return EXIT_SUCCESS;
}

} // namespace Tom::Commands

TINYORM_END_COMMON_NAMESPACE
