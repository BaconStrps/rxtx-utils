

namespace csics::radio {

class RadioRx {
    public:
        RadioRx() = default;
        virtual ~RadioRx() = default;

        virtual bool open() = 0;

};

};  // namespace csics::radio
