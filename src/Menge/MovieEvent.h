#	pragma once

#   include "MovieNodeExtra.h"

#   include "pybind/pybind.hpp"

namespace Menge
{
	class MovieEvent
		: public MovieNodeExtra
	{
    public:
        MovieEvent();
        ~MovieEvent();

    public:
		void setEvent( const pybind::object & _cb, const pybind::detail::args_operator_t & _args );
		void removeEvent();

    public:
        void setResourceMovie( const ResourceMoviePtr & _resourceMovie );

    protected:
        void _movieForwardUpdate( float _time, uint32_t _beginFrame, uint32_t _endFrame, const MovieLayer & _layer ) override;        

    protected:
		pybind::object m_cb;
		pybind::detail::args_operator_t m_args;

		ResourceMoviePtr m_resourceMovie;
	};
}